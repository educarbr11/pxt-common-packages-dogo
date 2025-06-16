#include "pxt.h"

// micro:bit dal
#if defined(MICROBIT_H) 

#define CODAL_RADIO MicroBitRadio
#define DEVICE_OK MICROBIT_OK
#define DEVICE_NOT_SUPPORTED MICROBIT_NOT_SUPPORTED
#define CODAL_EVENT MicroBitEvent
#define CODAL_RADIO_MICROBIT_DAL 1

// any other NRF52 board
#elif defined(NRF52_SERIES)

#include "NRF52Radio.h"
#define CODAL_RADIO codal::NRF52Radio
#define CODAL_EVENT codal::Event

#endif

using namespace pxt;

#ifndef MICROBIT_RADIO_MAX_PACKET_SIZE
#define MICROBIT_RADIO_MAX_PACKET_SIZE          32
#endif

#ifndef DEVICE_RADIO_MAX_PACKET_SIZE
#define DEVICE_RADIO_MAX_PACKET_SIZE MICROBIT_RADIO_MAX_PACKET_SIZE
#endif

#ifndef MICROBIT_ID_RADIO
#define MICROBIT_ID_RADIO               29
#endif

#ifndef DEVICE_ID_RADIO
#define DEVICE_ID_RADIO MICROBIT_ID_RADIO
#endif

#ifndef MICROBIT_RADIO_EVT_DATAGRAM
#define MICROBIT_RADIO_EVT_DATAGRAM             1       // Event to signal that a new datagram has been received.
#endif

#ifndef DEVICE_RADIO_EVT_DATAGRAM
#define DEVICE_RADIO_EVT_DATAGRAM MICROBIT_RADIO_EVT_DATAGRAM
#endif

//% color=#E3008C weight=96 icon="\uf012"
namespace radio {
    
#if CODAL_RADIO_MICROBIT_DAL
    CODAL_RADIO* getRadio() {
        return &uBit.radio;
    }
#elif defined(CODAL_RADIO)
class RadioWrap {
    CODAL_RADIO radio;
    public:
        RadioWrap() 
            : radio()
        {}

    CODAL_RADIO* getRadio() {
        return &radio;
    }
};
SINGLETON(RadioWrap);
CODAL_RADIO* getRadio() {
    auto wrap = getRadioWrap();
    if (NULL != wrap)
        return wrap->getRadio();    
    return NULL;
}
#endif // #else

    bool radioEnabled = false;
    bool init = false;
    int radioEnable() {
#ifdef CODAL_RADIO
        auto radio = getRadio();
        if (NULL == radio) 
            return DEVICE_NOT_SUPPORTED;

        if (init && !radioEnabled) {
            //If radio was explicitly disabled from a call to off API
            //We don't want to enable it here. User needs to call on API first.
            return DEVICE_NOT_SUPPORTED;
        }

        int r = radio->enable();
        if (r != DEVICE_OK) {
            target_panic(43);
            return r;
        }
        if (!init) {
            getRadio()->setGroup(0); //Default group zero. This used to be pxt::programHash()
            getRadio()->setTransmitPower(6); // start with high power by default
            init = true;
        }
        radioEnabled = true;
        return r;
#else
        return DEVICE_NOT_SUPPORTED;
#endif
    }

    /**
    * Disables the radio for use as a multipoint sender/receiver.
    * Disabling radio will help conserve battery power when it is not in use.
    */
    //% help=radio/off
    void off() {
#ifdef CODAL_RADIO
        auto radio = getRadio();
        if (NULL == radio)
            return;

        int r = radio->disable();
        if (r != DEVICE_OK) {
            target_panic(43);
        } else {
            radioEnabled = false;
        }
#else
        return;
#endif
    }

    /**
    * Initialises the radio for use as a multipoint sender/receiver
    * Only useful when the radio.off() is used beforehand.
    */
    //% help=radio/on
    void on() {
#ifdef CODAL_RADIO
        auto radio = getRadio();
        if (NULL == radio)
            return;

        int r = radio->enable();
        if (r != DEVICE_OK) {
            target_panic(43);
        } else {
            radioEnabled = true;
        }
#else
        return;
#endif
    }

    /**
    * Envia um evento via rádio para dispositivos vizinhos.
    */
    //% blockId=radioRaiseEvent block="rádio: enviar evento|da fonte %src=control_event_source_id|com valor %value=control_event_value_id"
    //% blockExternalInputs=1
    //% advanced=false
    //% weight=1
    //% help=radio/raise-event
    void raiseEvent(int src, int value) {
#ifdef CODAL_RADIO        
        if (radioEnable() != DEVICE_OK) return;

        getRadio()->event.eventReceived(CODAL_EVENT(src, value, CREATE_ONLY));
#endif        
    }

    /**
     * Internal use only. Takes the next packet from the radio queue and returns its contents + RSSI in a Buffer.
     * @returns NULL if no packet available
     */
    //%
    Buffer readRawPacket() {
#ifdef CODAL_RADIO        
        if (radioEnable() != DEVICE_OK) return NULL;

        auto p = getRadio()->datagram.recv();
#if CODAL_RADIO_MICROBIT_DAL
        if (p == PacketBuffer::EmptyPacket)
            return NULL;
        int rssi = p.getRSSI();
        auto length = p.length();
        auto bytes = p.getBytes();
#else
        // TODO: RSSI support
        int rssi = -73;        
        auto length = p.length();
        auto bytes = p.getBytes();
        if (length == 0)
            return NULL;
#endif

        uint8_t buf[DEVICE_RADIO_MAX_PACKET_SIZE + sizeof(int)]; // packet length + rssi
        memset(buf, 0, sizeof(buf));
        memcpy(buf, bytes, length); // data
        memcpy(buf + DEVICE_RADIO_MAX_PACKET_SIZE, &rssi, sizeof(int)); // RSSi - assumes Int32LE layout
        return mkBuffer(buf, sizeof(buf));
#else
        return NULL;
#endif        
    }

    /**
     * Internal use only. Sends a raw packet through the radio (assumes RSSI appened to packet)
     */
    //% async
    void sendRawPacket(Buffer msg) {
#ifdef CODAL_RADIO        
        if (radioEnable() != DEVICE_OK || NULL == msg) return;

        // don't send RSSI data; and make sure no buffer underflow
        int len = msg->length - sizeof(int);
        if (len > 0)
            getRadio()->datagram.send(msg->data, len);
#endif            
    }

    /**
     * Used internally by the library.
     */
    //% help=radio/on-data-received
    //% weight=0
    //% blockId=radio_datagram_received_event block="radio on data received" blockGap=8
    //% deprecated=true blockHidden=1
    void onDataReceived(Action body) {
#ifdef CODAL_RADIO        
        if (radioEnable() != DEVICE_OK) return;

        registerWithDal(DEVICE_ID_RADIO, DEVICE_RADIO_EVT_DATAGRAM, body);
        getRadio()->datagram.recv(); // wake up read code
#endif       
    }

    /**
    * Define o ID do grupo para comunicações por rádio. Um micro:bit só pode escutar um ID de grupo por vez.
    * @param id o ID do grupo, entre ``0`` e ``255``, ex: 1
     */
    //% help=radio/set-group
    //% weight=100
    //% blockId=radio_set_group block="definir canal de rádio %ID"
    //% id.min=0 id.max=255
    //% group="CANAL"
    void setGroup(int id) {
#ifdef CODAL_RADIO        
        if (radioEnable() != DEVICE_OK) return;

        getRadio()->setGroup(id);
#endif       
    }

   /**
    * Alterar o nível de potência de saída do transmissor para o valor especificado.
    * @param power um valor na faixa de 0 a 7, onde 0 é a potência mais baixa e 7 é a mais alta. ex: 7
    */
    //% help=radio/set-transmit-power
    //% weight=9 blockGap=8
    //% blockId=radio_set_transmit_power block="rádio: configurar potência de transmissão %power"
    //% power.min=0 power.max=7
    //% advanced=false
    void setTransmitPower(int power) {
#ifdef CODAL_RADIO        
        if (radioEnable() != DEVICE_OK) return;

        getRadio()->setTransmitPower(power);
#endif        
    }

    /**
    * Altere a banda de transmissão e recepção do rádio para o canal especificado.
    * @param band uma banda de frequência na faixa de 0 a 83. Cada passo equivale a 1MHz, começando em 2400MHz.
    **/
    //% help=radio/set-frequency-band
    //% weight=8 blockGap=8
    //% blockId=radio_set_frequency_band block="rádio: configurar banda de frequência %band"
    //% band.min=0 band.max=83
    //% advanced=false
    void setFrequencyBand(int band) {
#ifdef CODAL_RADIO        
        if (radioEnable() != DEVICE_OK) return;
        getRadio()->setFrequencyBand(band);
#endif        
    }
}
