#define ThingMacAdress ((const unit8_t *) "Your thing's name")
#define FactoryTopic_NAME ((const unit8_t*) "195/Factory/Instructions")
#define FactoryACK_STRING ((const char*) "ACK")
#define echoACK_STRING_LENGTH    4
#define echoMAX_DATA_LENGTH      20
#define echoDONT_BLOCK           ( ( TickType_t ) 0 )


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

/*-----------------------------------------------------------*/

static BaseType_t prvCreateClientAndConnectToBroker( void )
{
    MQTTAgentReturnCode_t xReturned;
    BaseType_t xReturn = pdFAIL;
    MQTTAgentConnectParams_t xConnectParameters =
    {
        clientcredentialMQTT_BROKER_ENDPOINT, /* The URL of the MQTT broker to connect to. */
        democonfigMQTT_AGENT_CONNECT_FLAGS,   /* Connection flags. */
        pdFALSE,                              /* Deprecated. */
        clientcredentialMQTT_BROKER_PORT,     /* Port number on which the MQTT broker is listening. Can be overridden by ALPN connection flag. */
        ThingMacAdress,                        /* Client Identifier of the MQTT client. It should be unique per broker. */
        0,                                    /* The length of the client Id, filled in later as not const. */
        pdFALSE,                              /* Deprecated. */
        NULL,                                 /* User data supplied to the callback. Can be NULL. */
        NULL,                                 /* Callback used to report various events. Can be NULL. */
        NULL,                                 /* Certificate used for secure connection. Can be NULL. */
        0                                     /* Size of certificate used for secure connection. */
    };

    /* Check this function has not already been executed. */
    configASSERT( xMQTTHandle == NULL );

    /* The MQTT client object must be created before it can be used.  The
     * maximum number of MQTT client objects that can exist simultaneously
     * is set by mqttconfigMAX_BROKERS. */
    xReturned = MQTT_AGENT_Create( &xMQTTHandle );

    if( xReturned == eMQTTAgentSuccess )
    {
        /* Fill in the MQTTAgentConnectParams_t member that is not const,
         * and therefore could not be set in the initializer (where
         * xConnectParameters is declared in this function). */
        xConnectParameters.usClientIdLength = ( uint16_t ) strlen( ( const char * ) echoCLIENT_ID );

        /* Connect to the broker. */
        configPRINTF( ( "MQTT echo attempting to connect to %s.\r\n", clientcredentialMQTT_BROKER_ENDPOINT ) );
        xReturned = MQTT_AGENT_Connect( xMQTTHandle,
                                        &xConnectParameters,
                                        democonfigMQTT_ECHO_TLS_NEGOTIATION_TIMEOUT );

        if( xReturned != eMQTTAgentSuccess )
        {
            /* Could not connect, so delete the MQTT client. */
            ( void ) MQTT_AGENT_Delete( xMQTTHandle );
            configPRINTF( ( "ERROR:  MQTT echo failed to connect with error %d.\r\n", xReturned ) );
        }
        else
        {
            configPRINTF( ( "MQTT echo connected.\r\n" ) );
            xReturn = pdPASS;
        }
    }

    return xReturn;
}

/*-----------------------------------------------------------*/
static void prvPublishNextMessage( BaseType_t xMessageNumber )
{
    MQTTAgentPublishParams_t xPublishParameters;
    MQTTAgentReturnCode_t xReturned;
    char cDataBuffer[ echoMAX_DATA_LENGTH ];

    /* Check this function is not being called before the MQTT client object has
     * been created. */
    configASSERT( xMQTTHandle != NULL );

    /* Create the message that will be published, which is of the form "Hello World n"
     * where n is a monotonically increasing number. Note that snprintf appends
     * terminating null character to the cDataBuffer. */
    ( void ) snprintf( cDataBuffer, echoMAX_DATA_LENGTH, "Hello World %d", ( int ) xMessageNumber );

    /* Setup the publish parameters. */
    memset( &( xPublishParameters ), 0x00, sizeof( xPublishParameters ) );
    xPublishParameters.pucTopic = FactoryTopic_NAME;
    xPublishParameters.pvData = cDataBuffer;
    xPublishParameters.usTopicLength = ( uint16_t ) strlen( ( const char * ) FactoryTopic_NAME );
    xPublishParameters.ulDataLength = ( uint32_t ) strlen( cDataBuffer );
    xPublishParameters.xQoS = eMQTTQoS1;

    /* Publish the message. */
    xReturned = MQTT_AGENT_Publish( xMQTTHandle,
                                    &( xPublishParameters ),
                                    democonfigMQTT_TIMEOUT );

    if( xReturned == eMQTTAgentSuccess )
    {
        configPRINTF( ( "Echo successfully published '%s'\r\n", cDataBuffer ) );
    }
    else
    {
        configPRINTF( ( "ERROR:  Echo failed to publish '%s'\r\n", cDataBuffer ) );
    }

    /* Remove compiler warnings in case configPRINTF() is not defined. */
    ( void ) xReturned;
}
/*-----------------------------------------------------------*/

static void prvPublishNextMessage( BaseType_t xMessageNumber )
{
    MQTTAgentPublishParams_t xPublishParameters;
    MQTTAgentReturnCode_t xReturned;
    char cDataBuffer[ echoMAX_DATA_LENGTH ];

    /* Check this function is not being called before the MQTT client object has
     * been created. */
    configASSERT( xMQTTHandle != NULL );

    /* Create the message that will be published, which is of the form "Hello World n"
     * where n is a monotonically increasing number. Note that snprintf appends
     * terminating null character to the cDataBuffer. */
    ( void ) snprintf( cDataBuffer, echoMAX_DATA_LENGTH, "Hello World %d", ( int ) xMessageNumber );

    /* Setup the publish parameters. */
    memset( &( xPublishParameters ), 0x00, sizeof( xPublishParameters ) );
    xPublishParameters.pucTopic = FactoryTopic_NAME;
    xPublishParameters.pvData = cDataBuffer;
    xPublishParameters.usTopicLength = ( uint16_t ) strlen( ( const char * ) FactoryTopic_NAME );
    xPublishParameters.ulDataLength = ( uint32_t ) strlen( cDataBuffer );
    xPublishParameters.xQoS = eMQTTQoS1;

    /* Publish the message. */
    xReturned = MQTT_AGENT_Publish( xMQTTHandle,
                                    &( xPublishParameters ),
                                    democonfigMQTT_TIMEOUT );

    if( xReturned == eMQTTAgentSuccess )
    {
        configPRINTF( ( "Echo successfully published '%s'\r\n", cDataBuffer ) );
    }
    else
    {
        configPRINTF( ( "ERROR:  Echo failed to publish '%s'\r\n", cDataBuffer ) );
    }

    /* Remove compiler warnings in case configPRINTF() is not defined. */
    ( void ) xReturned;
}
static void prvMessageEchoingTask( void * pvParameters )
{
    MQTTAgentPublishParams_t xPublishParameters;
    MQTTAgentReturnCode_t xReturned;
    char cDataBuffer[ echoMAX_DATA_LENGTH + echoACK_STRING_LENGTH ];
    size_t xBytesReceived;

    /* Remove compiler warnings about unused parameters. */
    ( void ) pvParameters;

    /* Check this task has not already been created. */
    configASSERT( xMQTTHandle != NULL );
    configASSERT( xEchoMessageBuffer != NULL );

    /* Setup the publish parameters. */
    xPublishParameters.pucTopic = echoTOPIC_NAME;
    xPublishParameters.usTopicLength = ( uint16_t ) strlen( ( const char * ) echoTOPIC_NAME );
    xPublishParameters.pvData = cDataBuffer;
    xPublishParameters.xQoS = eMQTTQoS1;

    for( ; ; )
    {
        /* Each message received on the message buffer has "ACK" appended to it
         * before being published on the same topic.  Wait for the next message. */
        memset( cDataBuffer, 0x00, sizeof( cDataBuffer ) );
        xBytesReceived = xMessageBufferReceive( xEchoMessageBuffer,
                                                cDataBuffer,
                                                sizeof( cDataBuffer ),
                                                portMAX_DELAY );

        /* Ensure the ACK can be added without overflowing the buffer.
        * Note that xBytesReceived already includes null character as
        * it is written to the message buffer in the MQTT callback. */
        if( xBytesReceived <= ( sizeof( cDataBuffer ) - ( size_t ) echoACK_STRING_LENGTH ) )
        {
            /* Append ACK to the received message. Note that
             * strcat appends terminating null character to the
             * cDataBuffer. */
            strcat( cDataBuffer, FactoryACK_STRING );
            xPublishParameters.ulDataLength = ( uint32_t ) strlen( cDataBuffer );

            /* Publish the ACK message. */
            xReturned = MQTT_AGENT_Publish( xMQTTHandle,
                                            &xPublishParameters,
                                            democonfigMQTT_TIMEOUT );

            if( xReturned == eMQTTAgentSuccess )
            {
                configPRINTF( ( "Message returned with ACK: '%s'\r\n", cDataBuffer ) );
            }
            else
            {
                configPRINTF( ( "ERROR:  Could not return message with ACK: '%s'\r\n", cDataBuffer ) );
            }
        }
        else
        {
            /* cDataBuffer is null terminated as the terminating null
             * character was sent from the MQTT callback. */
            configPRINTF( ( "ERROR:  Buffer is not big enough to return message with ACK: '%s'\r\n", cDataBuffer ) );
        }
    }
}
/*-----------------------------------------------------------*/
static void prvMQTTConnectAndPublishTask( void * pvParameters )
{
    BaseType_t xX;
    BaseType_t xReturned;
    const TickType_t xFiveSeconds = pdMS_TO_TICKS( 5000UL );
    const BaseType_t xIterationsInAMinute = 60 / 5;
    TaskHandle_t xEchoingTask = NULL;

    /* Avoid compiler warnings about unused parameters. */
    ( void ) pvParameters;

    /* Create the MQTT client object and connect it to the MQTT broker. */
    xReturned = prvCreateClientAndConnectToBroker();

    if( xReturned == pdPASS )
    {
        /* Create the task that echoes data received in the callback back to the
         * MQTT broker. */
        xReturned = xTaskCreate( prvMessageEchoingTask,               /* The function that implements the task. */
                                 "Echoing",                           /* Human readable name for the task. */
                                 democonfigMQTT_ECHO_TASK_STACK_SIZE, /* Size of the stack to allocate for the task, in words not bytes! */
                                 NULL,                                /* The task parameter is not used. */
                                 tskIDLE_PRIORITY,                    /* Runs at the lowest priority. */
                                 &( xEchoingTask ) );                 /* The handle is stored so the created task can be deleted again at the end of the demo. */

        if( xReturned != pdPASS )
        {
            /* The task could not be created because there was insufficient FreeRTOS
             * heap available to create the task's data structures and/or stack. */
            configPRINTF( ( "MQTT echoing task could not be created - out of heap space?\r\n" ) );
        }
    }
    else
    {
        configPRINTF( ( "MQTT echo test could not connect to broker.\r\n" ) );
    }

    if( xReturned == pdPASS )
    {
        configPRINTF( ( "MQTT echo test echoing task created.\r\n" ) );

        /* Subscribe to the echo topic. */
        xReturned = prvSubscribe();
    }

    if( xReturned == pdPASS )
    {
        /* MQTT client is now connected to a broker.  Publish a message
         * every five seconds until a minute has elapsed. */
        for( xX = 0; xX < xIterationsInAMinute; xX++ )
        {
            prvPublishNextMessage( xX );

            /* Five seconds delay between publishes. */
            vTaskDelay( xFiveSeconds );
        }
    }

    /* Disconnect the client. */
    ( void ) MQTT_AGENT_Disconnect( xMQTTHandle, democonfigMQTT_TIMEOUT );

    /* End the demo by deleting all created resources. */
    configPRINTF( ( "MQTT echo demo finished.\r\n" ) );
    configPRINTF( ( "----Demo finished----\r\n" ) );
    vMessageBufferDelete( xEchoMessageBuffer );
    vTaskDelete( xEchoingTask );
    vTaskDelete( NULL ); /* Delete this task. */
}
/*-----------------------------------------------------------*/
static BaseType_t prvSubscribe( void )
{
    MQTTAgentReturnCode_t xReturned;
    BaseType_t xReturn;
    MQTTAgentSubscribeParams_t xSubscribeParams;

    /* Setup subscribe parameters to subscribe to FactoryTopic_NAME topic. */
    xSubscribeParams.pucTopic = FactoryTopic_NAME;
    xSubscribeParams.pvPublishCallbackContext = NULL;
    xSubscribeParams.pxPublishCallback = prvMQTTCallback;
    xSubscribeParams.usTopicLength = ( uint16_t ) strlen( ( const char * ) FactoryTopic_NAME );
    xSubscribeParams.xQoS = eMQTTQoS1;

    /* Subscribe to the topic. */
    xReturned = MQTT_AGENT_Subscribe( xMQTTHandle,
                                      &xSubscribeParams,
                                      democonfigMQTT_TIMEOUT );

    if( xReturned == eMQTTAgentSuccess )
    {
        configPRINTF( ( "MQTT Echo demo subscribed to %s\r\n", FactoryTopic_NAME ) );
        xReturn = pdPASS;
    }
    else
    {
        configPRINTF( ( "ERROR:  MQTT Echo demo could not subscribe to %s\r\n", FactoryTopic_NAME ) );
        xReturn = pdFAIL;
    }

    return xReturn;
}
/*-----------------------------------------------------------*/

static MQTTBool_t prvMQTTCallback( void * pvUserData,
                                   const MQTTPublishData_t * const pxPublishParameters )
{
    char cBuffer[ echoMAX_DATA_LENGTH + echoACK_STRING_LENGTH ];
    uint32_t ulBytesToCopy = ( echoMAX_DATA_LENGTH + echoACK_STRING_LENGTH - 1 ); /* Bytes to copy initialized to ensure it
                                                                                   * fits in the buffer. One place is left
                                                                                   * for NULL terminator. */

    /* Remove warnings about the unused parameters. */
    ( void ) pvUserData;

    /* Don't expect the callback to be invoked for any other topics. */
    configASSERT( ( size_t ) ( pxPublishParameters->usTopicLength ) == strlen( ( const char * ) FactoryTopic_NAME ) );
    configASSERT( memcmp( pxPublishParameters->pucTopic, FactoryTopic_NAME, ( size_t ) ( pxPublishParameters->usTopicLength ) ) == 0 );

    /* THe ulBytesToCopy has already been initialized to ensure it does not copy
     * more bytes than will fit in the buffer.  Now check it does not copy more
     * bytes than are available. */
    if( pxPublishParameters->ulDataLength <= ulBytesToCopy )
    {
        ulBytesToCopy = pxPublishParameters->ulDataLength;

        /* Set the buffer to zero and copy the data into the buffer to ensure
         * there is a NULL terminator and the buffer can be accessed as a
         * string. */
        memset( cBuffer, 0x00, sizeof( cBuffer ) );
        memcpy( cBuffer, pxPublishParameters->pvData, ( size_t ) ulBytesToCopy );

        /* Only echo the message back if it has not already been echoed.  If the
         * data has already been echoed then it will already contain the echoACK_STRING
         * string. */
        if( strstr( cBuffer, FactoryACK_STRING ) == NULL )
        {
            /* The string has not been echoed before, so send it to the publish
             * task, which will then echo the data back.  Make sure to send the
             * terminating null character as well so that the received buffer in
             * EchoingTask can be printed as a C string.  THE DATA CANNOT BE ECHOED
             * BACK WITHIN THE CALLBACK AS THE CALLBACK IS EXECUTING WITHINT THE
             * CONTEXT OF THE MQTT TASK.  Calling an MQTT API function here could cause
             * a deadlock. */
            ( void ) xMessageBufferSend( xEchoMessageBuffer, cBuffer, ( size_t ) ulBytesToCopy + ( size_t ) 1, echoDONT_BLOCK );
        }
    }
    else
    {
        configPRINTF( ( "[WARN]: Dropping received message as it does not fit in the buffer.\r\n" ) );
    }

    /* The data was copied into the FreeRTOS message buffer, so the buffer
     * containing the data is no longer required.  Returning eMQTTFalse tells the
     * MQTT agent that the ownership of the buffer containing the message lies with
     * the agent and it is responsible for freeing the buffer. */
    return eMQTTFalse;
}
/*-----------------------------------------------------------*/