#include <libs/SkyRoad.h>

// Static storage locations for SkyRoad static variables.
SemaphoreHandle_t SkyRoad::mutex;
volatile uint32_t SkyRoad::anonymizer_inc = 0;
std::map<std::string, SkyRoad::Hermes*> * volatile SkyRoad::phonebook = NULL;;
StatCounter SkyRoad::Hermes::global_deliveries("skyroad.global.deliveries");
StatCounter SkyRoad::Hermes::global_blocking_deliveries("skyroad.global.blocking_deliveries");
LogTree *SkyRoad::log_root;
LogTree *SkyRoad::log_registration;
LogTree *SkyRoad::log_messengers;

/* Instantiate these to check for compile-time errors in the template classes.
 * If unused they will probably be optimized away by the linker.
 */
template class SkyRoad::Messenger<int>;
template class SkyRoad::Envelope::Scroll<int>;