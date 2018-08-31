#ifndef SRC_COMMON_UW_IPMC_SERVICES_IPMI_SDR_SENSORDATARECORDSENSOR_H_
#define SRC_COMMON_UW_IPMC_SERVICES_IPMI_SDR_SENSORDATARECORDSENSOR_H_

#include <services/ipmi/sdr/SensorDataRecord.h>
#include <map>
#include <string>

/**
 * A generic intermediary class providing the common interface between type 01,
 * type 02, type 03 SDRs.
 */
class SensorDataRecordSensor : public SensorDataRecord {
protected:
	/// Protected constructor since our type-code based accessors are technically virtual
	SensorDataRecordSensor(const std::vector<uint8_t> &sdr_data) : SensorDataRecord(sdr_data) { };
public:
	virtual ~SensorDataRecordSensor() { };

	virtual std::vector<uint8_t> get_record_key();
	virtual bool validate();
	virtual void initialize_blank(std::string name);

	/**
	 * SDR Data Accessors
	 *
	 * \warning Do not call any accessors on a record that does not validate().
	 */
	///@{
#define SDR_FIELD(name, type, byte, a, b) \
	virtual type name(); \
	virtual void name(type val);

	SDR_FIELD(sensor_owner_id, uint8_t, 5, 7, 0)
	SDR_FIELD(sensor_owner_channel, uint8_t, 6, 7, 4)
	SDR_FIELD(sensor_owner_lun, uint8_t, 6, 2, 0)
	SDR_FIELD(sensor_number, uint8_t, 7, 7, 0)

	SDR_FIELD(entity_id, uint8_t, 8, 7, 0)
	SDR_FIELD(entity_instance_is_container, bool, 9, 7, 7)
	SDR_FIELD(entity_instance, uint8_t, 9, 6, 0)

	SDR_FIELD(sensor_type_code, uint8_t, VARIABLE, 7, 0) // Different in Type 03
	SDR_FIELD(event_type_reading_code, uint8_t, VARIABLE, 7, 0) // Different in Type 03

	enum Direction {
		DIR_UNSPECIFIED = 0,
		DIR_INPUT       = 1,
		DIR_OUTPUT      = 2,
		DIR_RESERVED    = 3,
	};
	SDR_FIELD(sensor_direction, enum Direction, VARIABLE, 1, 0)

	SDR_FIELD(oem, uint8_t, VARIABLE, 7, 0)

	SDR_FIELD(id_string, std::string, VARIABLE, 7, 0)

#undef SDR_FIELD
protected:
	virtual uint8_t _get_id_string_offset();
	///@}
};

#endif /* SRC_COMMON_UW_IPMC_SERVICES_IPMI_SDR_SENSORDATARECORDSENSOR_H_ */
