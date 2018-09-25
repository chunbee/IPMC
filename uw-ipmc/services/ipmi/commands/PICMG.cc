#include <services/ipmi/IPMI.h>
#include <services/ipmi/ipmbsvc/IPMBSvc.h>
#include "IPMICmd_Index.h"

// AdvancedTCA

#define ASSERT_PICMG_IDENTIFIER(ipmb, message) \
	do { \
		if (message.data_len < 1 || message.data[0] != 0) { \
			std::shared_ptr<IPMI_MSG> assert_picmg_identifier_failed = std::make_shared<IPMI_MSG>(); \
			message.prepare_reply(*assert_picmg_identifier_failed); \
			assert_picmg_identifier_failed->data[0] = IPMI::Completion::Invalid_Data_Field_In_Request; \
			assert_picmg_identifier_failed->data_len = 1; \
			ipmb.send(assert_picmg_identifier_failed); \
			return; \
		} \
	} while (0)

static void ipmicmd_Get_PICMG_Properties(IPMBSvc &ipmb, const IPMI_MSG &message) {
	ASSERT_PICMG_IDENTIFIER(ipmb, message);
	std::shared_ptr<IPMI_MSG> reply = std::make_shared<IPMI_MSG>();
	message.prepare_reply(*reply);
	reply->data[0] = IPMI::Completion::Success;
	reply->data[1] = 0; // PICMG Identifier (spec requires 0)
	reply->data[2] = 0x32; // PICMG Extension Version
	reply->data[3] = 0; // Max FRU Device ID // TODO
	reply->data[4] = 0; // FRU Device ID for IPM Controller (spec requires 0)
	reply->data_len = 5;
	ipmb.send(reply);
}
IPMICMD_INDEX_REGISTER(Get_PICMG_Properties);

#if 0 // Unimplemented.
static void ipmicmd_Get_Address_Info(IPMBSvc &ipmb, const IPMI_MSG &message) {
	ASSERT_PICMG_IDENTIFIER(ipmb, message);
}
IPMICMD_INDEX_REGISTER(Get_Address_Info);
#endif

#if 0 // Unimplemented.
static void ipmicmd_Get_Shelf_Address_Info(IPMBSvc &ipmb, const IPMI_MSG &message) {
	ASSERT_PICMG_IDENTIFIER(ipmb, message);
}
IPMICMD_INDEX_REGISTER(Get_Shelf_Address_Info);
#endif

#if 0 // Unimplemented.
static void ipmicmd_Set_Shelf_Address_Info(IPMBSvc &ipmb, const IPMI_MSG &message) {
	ASSERT_PICMG_IDENTIFIER(ipmb, message);
}
IPMICMD_INDEX_REGISTER(Set_Shelf_Address_Info);
#endif

#if 0 // Unimplemented.
static void ipmicmd_FRU_Control(IPMBSvc &ipmb, const IPMI_MSG &message) {
	ASSERT_PICMG_IDENTIFIER(ipmb, message);
}
IPMICMD_INDEX_REGISTER(FRU_Control);
#endif

#if 0 // Unimplemented.
static void ipmicmd_Get_FRU_LED_Properties(IPMBSvc &ipmb, const IPMI_MSG &message) {
	ASSERT_PICMG_IDENTIFIER(ipmb, message);
}
IPMICMD_INDEX_REGISTER(Get_FRU_LED_Properties);
#endif

#if 0 // Unimplemented.
static void ipmicmd_Get_LED_Color_Capabilities(IPMBSvc &ipmb, const IPMI_MSG &message) {
	ASSERT_PICMG_IDENTIFIER(ipmb, message);
}
IPMICMD_INDEX_REGISTER(Get_LED_Color_Capabilities);
#endif

#if 0 // Unimplemented.
static void ipmicmd_Set_FRU_LED_State(IPMBSvc &ipmb, const IPMI_MSG &message) {
	ASSERT_PICMG_IDENTIFIER(ipmb, message);
}
IPMICMD_INDEX_REGISTER(Set_FRU_LED_State);
#endif

#if 0 // Unimplemented.
static void ipmicmd_Get_FRU_LED_State(IPMBSvc &ipmb, const IPMI_MSG &message) {
	ASSERT_PICMG_IDENTIFIER(ipmb, message);
}
IPMICMD_INDEX_REGISTER(Get_FRU_LED_State);
#endif

#if 0 // Unimplemented.
static void ipmicmd_Set_IPMB_State(IPMBSvc &ipmb, const IPMI_MSG &message) {
	ASSERT_PICMG_IDENTIFIER(ipmb, message);
}
IPMICMD_INDEX_REGISTER(Set_IPMB_State);
#endif

#if 0 // Unimplemented.
static void ipmicmd_Set_FRU_Activation_Policy(IPMBSvc &ipmb, const IPMI_MSG &message) {
	ASSERT_PICMG_IDENTIFIER(ipmb, message);
}
IPMICMD_INDEX_REGISTER(Set_FRU_Activation_Policy);
#endif

#if 0 // Unimplemented.
static void ipmicmd_Get_FRU_Activation_Policy(IPMBSvc &ipmb, const IPMI_MSG &message) {
	ASSERT_PICMG_IDENTIFIER(ipmb, message);
}
IPMICMD_INDEX_REGISTER(Get_FRU_Activation_Policy);
#endif

#if 0 // Unimplemented.
static void ipmicmd_Set_FRU_Activation(IPMBSvc &ipmb, const IPMI_MSG &message) {
	ASSERT_PICMG_IDENTIFIER(ipmb, message);
}
IPMICMD_INDEX_REGISTER(Set_FRU_Activation);
#endif

#if 0 // Unimplemented.
static void ipmicmd_Get_Device_Locator_Record_ID(IPMBSvc &ipmb, const IPMI_MSG &message) {
	ASSERT_PICMG_IDENTIFIER(ipmb, message);
	// TODO: Required: PICMG 3.0 REQ 3.352
}
IPMICMD_INDEX_REGISTER(Get_Device_Locator_Record_ID);
#endif

#if 0 // Unimplemented.
static void ipmicmd_Set_Port_State(IPMBSvc &ipmb, const IPMI_MSG &message) {
	ASSERT_PICMG_IDENTIFIER(ipmb, message);
}
IPMICMD_INDEX_REGISTER(Set_Port_State);
#endif

#if 0 // Unimplemented.
static void ipmicmd_Get_Port_State(IPMBSvc &ipmb, const IPMI_MSG &message) {
	ASSERT_PICMG_IDENTIFIER(ipmb, message);
}
IPMICMD_INDEX_REGISTER(Get_Port_State);
#endif

#if 0 // Unimplemented.
static void ipmicmd_Compute_Power_Properties(IPMBSvc &ipmb, const IPMI_MSG &message) {
	ASSERT_PICMG_IDENTIFIER(ipmb, message);
}
IPMICMD_INDEX_REGISTER(Compute_Power_Properties);
#endif

#if 0 // Unimplemented.
static void ipmicmd_Set_Power_Level(IPMBSvc &ipmb, const IPMI_MSG &message) {
	ASSERT_PICMG_IDENTIFIER(ipmb, message);
}
IPMICMD_INDEX_REGISTER(Set_Power_Level);
#endif

#if 0 // Unimplemented.
static void ipmicmd_Get_Power_Level(IPMBSvc &ipmb, const IPMI_MSG &message) {
	ASSERT_PICMG_IDENTIFIER(ipmb, message);
}
IPMICMD_INDEX_REGISTER(Get_Power_Level);
#endif

#if 0 // Unimplemented.
static void ipmicmd_Renegotiate_Power(IPMBSvc &ipmb, const IPMI_MSG &message) {
	ASSERT_PICMG_IDENTIFIER(ipmb, message);
}
IPMICMD_INDEX_REGISTER(Renegotiate_Power);
#endif

#if 0 // Unimplemented.
static void ipmicmd_Get_Fan_Speed_Properties(IPMBSvc &ipmb, const IPMI_MSG &message) {
	ASSERT_PICMG_IDENTIFIER(ipmb, message);
}
IPMICMD_INDEX_REGISTER(Get_Fan_Speed_Properties);
#endif

#if 0 // Unimplemented.
static void ipmicmd_Set_Fan_Level(IPMBSvc &ipmb, const IPMI_MSG &message) {
	ASSERT_PICMG_IDENTIFIER(ipmb, message);
}
IPMICMD_INDEX_REGISTER(Set_Fan_Level);
#endif

#if 0 // Unimplemented.
static void ipmicmd_Get_Fan_Level(IPMBSvc &ipmb, const IPMI_MSG &message) {
	ASSERT_PICMG_IDENTIFIER(ipmb, message);
}
IPMICMD_INDEX_REGISTER(Get_Fan_Level);
#endif

#if 0 // Unimplemented.
static void ipmicmd_Bused_Resource(IPMBSvc &ipmb, const IPMI_MSG &message) {
	ASSERT_PICMG_IDENTIFIER(ipmb, message);
}
IPMICMD_INDEX_REGISTER(Bused_Resource);
#endif

#if 0 // Unimplemented.
static void ipmicmd_Get_IPMB_Link_Info(IPMBSvc &ipmb, const IPMI_MSG &message) {
	ASSERT_PICMG_IDENTIFIER(ipmb, message);
}
IPMICMD_INDEX_REGISTER(Get_IPMB_Link_Info);
#endif

#if 0 // Unimplemented.
static void ipmicmd_Get_Shelf_Manager_IPMB_Address(IPMBSvc &ipmb, const IPMI_MSG &message) {
	ASSERT_PICMG_IDENTIFIER(ipmb, message);
}
IPMICMD_INDEX_REGISTER(Get_Shelf_Manager_IPMB_Address);
#endif

#if 0 // Unimplemented.
static void ipmicmd_Set_Fan_Policy(IPMBSvc &ipmb, const IPMI_MSG &message) {
	ASSERT_PICMG_IDENTIFIER(ipmb, message);
}
IPMICMD_INDEX_REGISTER(Set_Fan_Policy);
#endif

#if 0 // Unimplemented.
static void ipmicmd_Get_Fan_Policy(IPMBSvc &ipmb, const IPMI_MSG &message) {
	ASSERT_PICMG_IDENTIFIER(ipmb, message);
}
IPMICMD_INDEX_REGISTER(Get_Fan_Policy);
#endif

#if 0 // Unimplemented.
static void ipmicmd_FRU_Control_Capabilities(IPMBSvc &ipmb, const IPMI_MSG &message) {
	ASSERT_PICMG_IDENTIFIER(ipmb, message);
}
IPMICMD_INDEX_REGISTER(FRU_Control_Capabilities);
#endif

#if 0 // Unimplemented.
static void ipmicmd_FRU_Inventory_Device_Lock_Control(IPMBSvc &ipmb, const IPMI_MSG &message) {
	ASSERT_PICMG_IDENTIFIER(ipmb, message);
}
IPMICMD_INDEX_REGISTER(FRU_Inventory_Device_Lock_Control);
#endif

#if 0 // Unimplemented.
static void ipmicmd_FRU_Inventory_Device_Write(IPMBSvc &ipmb, const IPMI_MSG &message) {
	ASSERT_PICMG_IDENTIFIER(ipmb, message);
}
IPMICMD_INDEX_REGISTER(FRU_Inventory_Device_Write);
#endif

#if 0 // Unimplemented.
static void ipmicmd_Get_Shelf_Manager_IP_Addresses(IPMBSvc &ipmb, const IPMI_MSG &message) {
	ASSERT_PICMG_IDENTIFIER(ipmb, message);
}
IPMICMD_INDEX_REGISTER(Get_Shelf_Manager_IP_Addresses);
#endif

#if 0 // Unimplemented.
static void ipmicmd_Get_Shelf_Power_Allocation(IPMBSvc &ipmb, const IPMI_MSG &message) {
	ASSERT_PICMG_IDENTIFIER(ipmb, message);
}
IPMICMD_INDEX_REGISTER(Get_Shelf_Power_Allocation);
#endif

#if 0 // Unimplemented.
static void ipmicmd_Get_Telco_Alarm_Capability(IPMBSvc &ipmb, const IPMI_MSG &message) {
	ASSERT_PICMG_IDENTIFIER(ipmb, message);
}
IPMICMD_INDEX_REGISTER(Get_Telco_Alarm_Capability);
#endif

#if 0 // Unimplemented.
static void ipmicmd_Set_Telco_Alarm_State(IPMBSvc &ipmb, const IPMI_MSG &message) {
	ASSERT_PICMG_IDENTIFIER(ipmb, message);
}
IPMICMD_INDEX_REGISTER(Set_Telco_Alarm_State);
#endif

#if 0 // Unimplemented.
static void ipmicmd_Get_Telco_Alarm_State(IPMBSvc &ipmb, const IPMI_MSG &message) {
	ASSERT_PICMG_IDENTIFIER(ipmb, message);
}
IPMICMD_INDEX_REGISTER(Get_Telco_Alarm_State);
#endif

#if 0 // Unimplemented.
static void ipmicmd_Get_Target_Upgrade_Capabilities(IPMBSvc &ipmb, const IPMI_MSG &message) {
    ASSERT_PICMG_IDENTIFIER(ipmb, message);
}
IPMICMD_INDEX_REGISTER(Get_Target_Upgrade_Capabilities);
#endif

#if 0 // Unimplemented.
static void ipmicmd_Get_Component_Properties(IPMBSvc &ipmb, const IPMI_MSG &message) {
    ASSERT_PICMG_IDENTIFIER(ipmb, message);
}
IPMICMD_INDEX_REGISTER(Get_Component_Properties);
#endif

#if 0 // Unimplemented.
static void ipmicmd_Abort_Firmware_Upgrade(IPMBSvc &ipmb, const IPMI_MSG &message) {
    ASSERT_PICMG_IDENTIFIER(ipmb, message);
}
IPMICMD_INDEX_REGISTER(Abort_Firmware_Upgrade);
#endif

#if 0 // Unimplemented.
static void ipmicmd_Initiate_Upgrade_Action(IPMBSvc &ipmb, const IPMI_MSG &message) {
    ASSERT_PICMG_IDENTIFIER(ipmb, message);
}
IPMICMD_INDEX_REGISTER(Initiate_Upgrade_Action);
#endif

#if 0 // Unimplemented.
static void ipmicmd_Upload_Firmware_Block(IPMBSvc &ipmb, const IPMI_MSG &message) {
    ASSERT_PICMG_IDENTIFIER(ipmb, message);
}
IPMICMD_INDEX_REGISTER(Upload_Firmware_Block);
#endif

#if 0 // Unimplemented.
static void ipmicmd_Finish_Firmware_Upload(IPMBSvc &ipmb, const IPMI_MSG &message) {
    ASSERT_PICMG_IDENTIFIER(ipmb, message);
}
IPMICMD_INDEX_REGISTER(Finish_Firmware_Upload);
#endif

#if 0 // Unimplemented.
static void ipmicmd_Get_Upgrade_Status(IPMBSvc &ipmb, const IPMI_MSG &message) {
    ASSERT_PICMG_IDENTIFIER(ipmb, message);
}
IPMICMD_INDEX_REGISTER(Get_Upgrade_Status);
#endif

#if 0 // Unimplemented.
static void ipmicmd_Activate_Firmware(IPMBSvc &ipmb, const IPMI_MSG &message) {
    ASSERT_PICMG_IDENTIFIER(ipmb, message);
}
IPMICMD_INDEX_REGISTER(Activate_Firmware);
#endif

#if 0 // Unimplemented.
static void ipmicmd_Query_Self_Test_Results(IPMBSvc &ipmb, const IPMI_MSG &message) {
    ASSERT_PICMG_IDENTIFIER(ipmb, message);
}
IPMICMD_INDEX_REGISTER(Query_Self_Test_Results);
#endif

#if 0 // Unimplemented.
static void ipmicmd_Query_Rollback_Status(IPMBSvc &ipmb, const IPMI_MSG &message) {
    ASSERT_PICMG_IDENTIFIER(ipmb, message);
}
IPMICMD_INDEX_REGISTER(Query_Rollback_Status);
#endif

#if 0 // Unimplemented.
static void ipmicmd_Initiate_Manual_Rollback(IPMBSvc &ipmb, const IPMI_MSG &message) {
    ASSERT_PICMG_IDENTIFIER(ipmb, message);
}
IPMICMD_INDEX_REGISTER(Initiate_Manual_Rollback);
#endif

#if 0 // Unimplemented.
static void ipmicmd_Get_Telco_Alarm_Location(IPMBSvc &ipmb, const IPMI_MSG &message) {
	ASSERT_PICMG_IDENTIFIER(ipmb, message);
}
IPMICMD_INDEX_REGISTER(Get_Telco_Alarm_Location);
#endif

#if 0 // Unimplemented.
static void ipmicmd_Set_FRU_Extracted(IPMBSvc &ipmb, const IPMI_MSG &message) {
	ASSERT_PICMG_IDENTIFIER(ipmb, message);
}
IPMICMD_INDEX_REGISTER(Set_FRU_Extracted);
#endif

#if 0 // Unimplemented.
static void ipmicmd_Get_HPM_x_Capabilities(IPMBSvc &ipmb, const IPMI_MSG &message) {
    ASSERT_PICMG_IDENTIFIER(ipmb, message);
}
IPMICMD_INDEX_REGISTER(Get_HPM_x_Capabilities);
#endif

#if 0 // Unimplemented.
static void ipmicmd_Get_Dynamic_Credentials(IPMBSvc &ipmb, const IPMI_MSG &message) {
    ASSERT_PICMG_IDENTIFIER(ipmb, message);
}
IPMICMD_INDEX_REGISTER(Get_Dynamic_Credentials);
#endif

#if 0 // Unimplemented.
static void ipmicmd_Get_Session_Handle_for_Explicit_LAN_Bridging(IPMBSvc &ipmb, const IPMI_MSG &message) {
    ASSERT_PICMG_IDENTIFIER(ipmb, message);
}
IPMICMD_INDEX_REGISTER(Get_Session_Handle_for_Explicit_LAN_Bridging);
#endif

#if 0 // Unimplemented.
static void ipmicmd_Get_ATCA_Extended_Management_Resources(IPMBSvc &ipmb, const IPMI_MSG &message) {
    ASSERT_PICMG_IDENTIFIER(ipmb, message);
}
IPMICMD_INDEX_REGISTER(Get_ATCA_Extended_Management_Resources);
#endif

#if 0 // Unimplemented.
static void ipmicmd_Get_AMC_Extended_Management_Resources(IPMBSvc &ipmb, const IPMI_MSG &message) {
    ASSERT_PICMG_IDENTIFIER(ipmb, message);
}
IPMICMD_INDEX_REGISTER(Get_AMC_Extended_Management_Resources);
#endif

#if 0 // Unimplemented.
static void ipmicmd_Set_ATCA_Extended_Management_State(IPMBSvc &ipmb, const IPMI_MSG &message) {
    ASSERT_PICMG_IDENTIFIER(ipmb, message);
}
IPMICMD_INDEX_REGISTER(Set_ATCA_Extended_Management_State);
#endif

#if 0 // Unimplemented.
static void ipmicmd_Get_ATCA_Extended_Management_State(IPMBSvc &ipmb, const IPMI_MSG &message) {
    ASSERT_PICMG_IDENTIFIER(ipmb, message);
}
IPMICMD_INDEX_REGISTER(Get_ATCA_Extended_Management_State);
#endif

#if 0 // Unimplemented.
static void ipmicmd_Set_AMC_Power_State(IPMBSvc &ipmb, const IPMI_MSG &message) {
    ASSERT_PICMG_IDENTIFIER(ipmb, message);
}
IPMICMD_INDEX_REGISTER(Set_AMC_Power_State);
#endif

#if 0 // Unimplemented.
static void ipmicmd_Get_AMC_Power_State(IPMBSvc &ipmb, const IPMI_MSG &message) {
    ASSERT_PICMG_IDENTIFIER(ipmb, message);
}
IPMICMD_INDEX_REGISTER(Get_AMC_Power_State);
#endif

#if 0 // Unimplemented.
static void ipmicmd_Assign_SOL_Payload_Instance(IPMBSvc &ipmb, const IPMI_MSG &message) {
    ASSERT_PICMG_IDENTIFIER(ipmb, message);
}
IPMICMD_INDEX_REGISTER(Assign_SOL_Payload_Instance);
#endif

#if 0 // Unimplemented.
static void ipmicmd_Get_IP_Address_Source(IPMBSvc &ipmb, const IPMI_MSG &message) {
    ASSERT_PICMG_IDENTIFIER(ipmb, message);
}
IPMICMD_INDEX_REGISTER(Get_IP_Address_Source);
#endif