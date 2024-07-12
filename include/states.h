typedef enum
    {
        START,
        SIMON,
        AWAITING_INPUT,
        HANDLING_INPUT,
        SUCCESS,
        FAIL,
        SCORE
    } STATES;

typedef enum
    {
        AWAITING_COMMAND,
        AWAITING_PAYLOAD,
    } SERIAL_STATES;