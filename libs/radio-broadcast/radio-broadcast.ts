namespace radio {
    const BROADCAST_GENERAL_ID = 2000;

    /**
     * Pega a messagem de rádio
     */
    //% blockHidden=1 shim=ENUM_GET
    //% blockId=radioMessageCode block="$msg" enumInitialMembers="message1"
    //% enumName=RadioMessage enumMemberName=msg enumPromptHint="e.g. Start, Stop, Jump..."
    //% enumIsHash=1
    export function __message(msg: number): number {
        return msg;
    }

    /**
     * Transmite uma mensagem pelo rádio
     * @param msg 
     */
    //% blockId=radioBroadcastMessage block="rádio: enviar mensagem $msg"
    //% msg.shadow=radioMessageCode draggableParameters
    //% weight=200
    //% blockGap=8
    //% help=radio/send-message
    //% group="Broadcast"
    export function sendMessage(msg: number): void {
        // 0 is MICROBIT_EVT_ANY, shifting by 1
        radio.raiseEvent(BROADCAST_GENERAL_ID, msg + 1);
    }

    /**
     * Registra o código a ser executado para uma mensagem específica
     * @param msg 
     * @param handler 
     */
    //% blockId=radioOnMessageReceived block="quando o rádio receber $msg"
    //% msg.shadow=radioMessageCode draggableParameters
    //% weight=199
    //% help=radio/on-received-message
    //% group="Broadcast"
    export function onReceivedMessage(msg: number, handler: () => void) {
        control.onEvent(BROADCAST_GENERAL_ID, msg + 1, handler);
    }
}