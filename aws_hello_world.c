#define ThingMacAdress ((const unit8_t *) "Your thing's name")
#define FactoryTopic ((const unit8_t*) "195/Factory/Instructions")
#define FactoryACK ((const char*) "ACK")

/**The FreeRTOS message buffer that is used to send data from the callback
 * function (see prvMQTTCallback() below) to the task sends the data back to
 * the broker. */
static MessageBufferHandle_t xMessageBuffer = NULL;

/**The handle of the MQTT client object used by the MQTT echo demo.
 */
static MQTTAgentHandle_t xMQTTHandle = NULL;

//MAIN FUCNTION DECLARATION
/**This function implements a task that connects to and then publishes messages 
 * to the MQTT broker. */
static void prvMQTTConnectAndPublishTask(void *pvParameter);

/**This function creates a MQTT client and connects to the MQTT broker. */
static void prvCreateClientAndConnectToBroker(void);

/**This function publishes the next message based on the message index to the topic
 * @param[in] xMessageNumber Appended to the message to make it unique.
*/
static void prvPublishNextMessage(BaseType_t xMessageNumber);

/**This function registered with MQTT broker and recieved message back
 * @param[in] pvUserData User data as supplied while registering the callback.
 * @param[in] pxCallbackParams Data received from the broker. */
static void prvMQTTCallback (void *pvUserData, const MQTTPublishData_t* pxCallbackParams);

/**This function subcribes to a topic. 
 * @return pdPASS if subscribe operation is successful, pdFALSE otherwise. */
static void prvSubscribe( void ); 

