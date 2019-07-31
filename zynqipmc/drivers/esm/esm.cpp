/*
 * This file is part of the ZYNQ-IPMC Framework.
 *
 * The ZYNQ-IPMC Framework is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * The ZYNQ-IPMC Framework is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the ZYNQ-IPMC Framework.  If not, see <https://www.gnu.org/licenses/>.
 */

/** In case this becomes necessary, this is the best way to disable ESM but leave the flash active:
 *
 * 	this->flash_reset->deassert();
 *  this->esm_reset->assert();
 *  vTaskDelay(pdMS_TO_TICKS(100));
 *
 *  And this is the best way to go back:
 *
 *  vTaskDelay(pdMS_TO_TICKS(200));
 *  this->esm_reset->release();
 *  vTaskDelay(pdMS_TO_TICKS(500));
 *  this->flash_reset->release();
 **/

#include "esm.h"

const std::string ESM::commandStatusToString(const CommandStatus& s) {
	switch (s) {
	case ESM_CMD_NOCOMMAND: return "No command to send";
	case ESM_CMD_NORESPONSE: return "No response";
	case ESM_CMD_OVERFLOW: return "Abnormal number of characters received";
	default: return "Success";
	}
}

ESM::ESM(UART &uart, ResetPin *esm_reset, Flash *flash, ResetPin *flash_reset) :
uart(uart), esm_reset(esm_reset), flash(flash), flash_reset(flash_reset) {
	this->mutex = xSemaphoreCreateMutex();
	configASSERT(this->mutex);
}

ESM::~ESM() {
	vSemaphoreDelete(this->mutex);
}

VFS::File ESM::createFlashFile() {
	if (!this->flash) {
		return VFS::File(nullptr, nullptr, 0);
	}

	return VFS::File(
		[this](uint8_t *buffer, size_t size) -> size_t {
			// Read
			MutexGuard<false> lock(this->mutex, true);
			this->flash->initialize();
			this->flash->read(0, buffer, size);
			return size;
		},
		[this](uint8_t *buffer, size_t size) -> size_t {
			MutexGuard<false> lock(this->mutex, true);
			this->flash->initialize();

			// Write
			if (!this->flash->write(0, buffer, size))
				return 0; // Failed to write

			// Write successful
			return size;
		}, 256 * 1024);
}

ESM::CommandStatus ESM::command(const std::string& command, std::string& response) {
	// Check if there is a command to send
	if (command == "") {
		return ESM_CMD_NOCOMMAND;
	}

	// Terminate with '/r' to trigger ESM to respond
	std::string formated_cmd = command + "\r";

	MutexGuard<false> lock(this->mutex, true);

	// Clear the receiver buffer
	this->uart.clear();

	// Send the command
	this->uart.write((const uint8_t*)formated_cmd.c_str(), formated_cmd.length(), pdMS_TO_TICKS(1000));

	// Read the incoming response
	// A single read such as:
	// size_t count = uart.read((uint8_t*)buf, 2048, pdMS_TO_TICKS(1000));
	// .. works but reading one character at a time allows us to detect
	// the end of the response which is '\r\n>'.
	char inbuf[2048] = "";
	size_t pos = 0, count = 0;
	while (pos < 2043) {
		count = this->uart.read((uint8_t*)inbuf+pos, 1, pdMS_TO_TICKS(1000));
		if (count == 0) break; // No character received
		if ((pos > 3) && (memcmp(inbuf+pos-2, "\r\n>", 3) == 0)) break; // End of command
		pos++;
	}

	if (pos == 0) {
		return ESM_CMD_NORESPONSE;
	} else if (pos == 2043) {
		return ESM_CMD_OVERFLOW;
	} else {
		// ESM will send back the command written and a new line.
		// At the end it will return '\r\n>', we erase this.
		size_t start = command.length() + 3;
		size_t end = strlen(inbuf);
		if (start >= end)
			return ESM_CMD_NORESPONSE;

		// Force the end of buf to be before '\r\n>'
		if (end > 3) { // We don't want a data abort here..
			inbuf[end-3] = '\0';
		}

		response = std::string(inbuf + start);
	}

	return ESM_CMD_SUCCESS;
}

bool ESM::getTemperature(float &temperature) {
	std::string response = "";

	CommandStatus r = this->command("-", response);

	if (r == ESM_CMD_SUCCESS) {
		std::vector<std::string> vs = stringSplit(response, ' ');

		if (vs.size() == 2 && !vs[0].compare("TEMP")) {
			int temp_raw = std::stoi(vs[1]);
			temperature = 177.4 - 0.8777 * temp_raw;
			return true;
		}
	};

	return false;
}

void ESM::restart() {
	if (this->esm_reset){
		MutexGuard<false> lock(this->mutex, true);
		esm_reset->toggle();
	} else {
		std::string resp;
		this->command("X", resp);
	}
	vTaskDelay(pdMS_TO_TICKS(1000));
}

//! A "esm.command" console command.
class ESM::Command : public CommandParser::Command {
public:
	Command(ESM &esm) : esm(esm) { };

	virtual std::string get_helptext(const std::string &command) const {
		return command + "\n\n"
				"Send a command to the ESM and see its output. Use ? to see possible commands.\n";
	}

	virtual void execute(std::shared_ptr<ConsoleSvc> console, const CommandParser::CommandParameters &parameters) {
		// Prepare the command
		int argn = parameters.nargs();
		std::string command = "", response, p;
		for (int i = 1; i < argn; i++) {
			parameters.parseParameters(i, true, &p);
			if (i == (argn-1)) {
				command += p;
			} else {
				command += p + " ";
			}
		}

		ESM::CommandStatus s = esm.command(command, response);

		if (s != ESM::ESM_CMD_SUCCESS) {
			console->write(ESM::commandStatusToString(s) + ".\n");
		} else {
			console->write(response);
		}
	}

private:
	ESM &esm; ///< ESM object.
};

//! A "esm.restart" console command.
class ESM::Restart : public CommandParser::Command {
public:
	Restart(ESM &esm) : esm(esm) { };

	virtual std::string get_helptext(const std::string &command) const {
		return command + "\n\n"
				"Restart the ESM module. Network interface will go down while restart is in progress.\n";
	}

	virtual void execute(std::shared_ptr<ConsoleSvc> console, const CommandParser::CommandParameters &parameters) {
		esm.restart();
	}

private:
	ESM &esm; ///< ESM object.
};

/// A "esm.flash.info" console command.
class ESM::FlashInfo : public CommandParser::Command {
public:
	FlashInfo(ESM &esm) : esm(esm) { };

	virtual std::string get_helptext(const std::string &command) const {
		return command + "\n\n"
				"Show information about the ESM flash. Network will go down if it is the first time accessing the flash.\n";
	}

	virtual void execute(std::shared_ptr<ConsoleSvc> console, const CommandParser::CommandParameters &parameters) {
		MutexGuard<false> lock(esm.mutex, true);

		if (!this->esm.flash->isInitialized()) {
			this->esm.flash->initialize();
		}

		console->write("Total flash size: " + bytesToString(esm.flash->getTotalSize()) + "\n");
	}

private:
	ESM &esm; ///< ESM object.
};

void ESM::registerConsoleCommands(CommandParser &parser, const std::string &prefix) {
	parser.registerCommand(prefix + "command", std::make_shared<ESM::Command>(*this));
	parser.registerCommand(prefix + "restart", std::make_shared<ESM::Restart>(*this));
	if (this->isFlashPresent())
		parser.registerCommand(prefix + "flash.info", std::make_shared<ESM::FlashInfo>(*this));
}

void ESM::deregisterConsoleCommands(CommandParser &parser, const std::string &prefix) {
	parser.registerCommand(prefix + "command", nullptr);
	parser.registerCommand(prefix + "restart", nullptr);
	if (this->isFlashPresent())
		parser.registerCommand(prefix + "flash.info", nullptr);
}