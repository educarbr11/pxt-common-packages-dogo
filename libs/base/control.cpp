#include "pxtbase.h"


namespace control {
    /**
    * Obtém o número de milissegundos que se passaram desde o momento em que o sistema foi ligado.
    */
    //% help=control/millis weight=50
    //% blockId=control_running_time block="millis (ms)"
    int millis() {
        return current_time_ms();
    }

    /**
    * Gets current time in microseconds. Overflows every ~18 minutes.
    */
    //%
    int micros() {
        return current_time_us() & 0x3fffffff;
    }

    /**
    * Used internally
    */
    //%
    void internalOnEvent(int src, int value, Action handler, int flags = 16) {
        registerWithDal(src, value, handler, flags);
    }

    /**
     * Reset the device.
     */
    //% weight=30 async help=control/reset blockGap=8
    //% blockId="control_reset" block="reiniciar"
    void reset() {
        target_reset();
    }

    /**
    * Bloqueia a fibra atual pelo número de microssegundos especificado
    * @param micros número de microssegundos para aguardar. Exemplo: 4
    */
    //% help=control/wait-micros weight=29 async
    //% blockId="control_wait_us" block="esperar (µs)%micros"
    void waitMicros(int micros) {
        sleep_us(micros);
    }

    /**
     * Run other code in the parallel.
     */
    //% help=control/run-in-parallel handlerStatement=1
    //% blockId="control_run_in_parallel" block="run in parallel" blockGap=8
    void runInParallel(Action a) {
        pxt::runInParallel(a);
    }

    /**
    * Bloqueia a thread de chamada até que o evento especificado seja acionado.
    */
    //% help=control/wait-for-event async
    //% blockId=control_wait_for_event block="esperar evento de %src|com o valor %value"
    void waitForEvent(int src, int value) {
        pxt::waitForEvent(src, value);
    }

    /**
    * Derive a unique, consistent serial number of this device from internal data.
    */
    //% blockId="control_device_serial_number" block="device serial number" weight=9
    //% help=control/device-serial-number
    int deviceSerialNumber() {
        uint64_t serial_num = pxt::getLongSerialNumber();
        return hash_fnv1(&serial_num, sizeof(serial_num)) & 0x3fffffff;
    }

    /**
    * Derive a unique, consistent 64-bit serial number of this device from internal data.
    */
    //% blockId="control_device_long_serial_number" block="device long serial number" weight=9
    //% help=control/device-long-serial-number
    Buffer deviceLongSerialNumber() {
        uint64_t serial_num = pxt::getLongSerialNumber();
        return mkBuffer((uint8_t*)&serial_num, sizeof(uint64_t));
    }

    /**
    *
    */
    //%
    void __log(int prority, String text) {
        if (NULL == text) return;
        pxt::sendSerial(text->getUTF8Data(), text->getUTF8Size());
    }

    /**
     * Dump internal information about a value.
     */
    //%
    void dmesgValue(TValue v) {
        anyPrint(v);
    }
}
