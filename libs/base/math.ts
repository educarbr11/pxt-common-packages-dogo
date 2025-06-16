namespace Math {

    export const E = 2.718281828459045;
    export const LN2 = 0.6931471805599453;
    export const LN10 = 2.302585092994046;
    export const LOG2E = 1.4426950408889634;
    export const LOG10E = 0.4342944819032518;
    export const PI = 3.141592653589793;
    export const SQRT1_2 = 0.7071067811865476;
    export const SQRT2 = 1.4142135623730951;

    /**
     * Re-mapeia um número de um intervalo para outro. Por exemplo, um valor no intervalo "de mínimo" será mapeado para "para mínimo", um valor no intervalo "de máximo" será mapeado para "para máximo", e valores intermediários serão ajustados proporcionalmente.
     * @param value valor a ser mapeado entre os intervalos
     * @param fromLow o limite inferior do intervalo atual do valor
     * @param fromHigh o limite superior do intervalo atual do valor, ex: 1023
     * @param toLow o limite inferior do intervalo de destino
     * @param toHigh o limite superior do intervalo de destino, ex: 4
     */
    //% help=math/map weight=10 blockGap=8
    //% blockId=math_map block="tranformar %value|de mínimo A %fromLow|máximo A %fromHigh|para mínimo B %toLow|máximo B %toHigh"
    //% inlineInputMode=inline
    export function map(value: number, fromLow: number, fromHigh: number, toLow: number, toHigh: number): number {
        return ((value - fromLow) * (toHigh - toLow)) / (fromHigh - fromLow) + toLow;
    }    

    /**
     * Restringe um número para estar dentro de um intervalo específico.
     * @param value o número a ser limitado, todos os tipos de dados são aceitos
     * @param low o limite inferior do intervalo, todos os tipos de dados são aceitos
     * @param high o limite superior do intervalo, todos os tipos de dados são aceitos
     */
    //% help=math/constrain weight=11 blockGap=8
    //% blockId="math_constrain_value" block="restringir %value|entre %low|e %high"
    export function constrain(value: number, low: number, high: number): number {
        return value < low ? low : value > high ? high : value;
    }

    const b_m16: number[] = [0, 49, 49, 41, 90, 27, 117, 10]
    /**
     * Returns the sine of an input angle. This is an 8-bit approximation.
     * @param theta input angle from 0-255
     */
    //% help=math/isin weight=11 advanced=true blockGap=8
    export function isin(theta: number) {
        //reference: based on FASTLed's sin approximation method: [https://github.com/FastLED/FastLED](MIT)
        let offset = theta;
        if( theta & 0x40 ) {
            offset = 255 - offset;
        }
        offset &= 0x3F; // 0..63

        let secoffset  = offset & 0x0F; // 0..15
        if( theta & 0x40) secoffset++;

        let section = offset >> 4; // 0..3
        let s2 = section * 2;

        let b = b_m16[s2];
        let m16 = b_m16[s2+1];
        let mx = (m16 * secoffset) >> 4;
        
        let y = mx + b;
        if( theta & 0x80 ) y = -y;

        y += 128;

        return y;
    }

    /**
     * Returns the cosine of an input angle. This is an 8-bit approximation. 
     * @param theta input angle from 0-255
     */
    //% help=math/icos weight=10 advanced=true blockGap=8
    export function icos(theta: number) {
        return isin(theta + 16384);
    }
}

namespace Number {
    export const EPSILON = 2.220446049250313e-16;
}