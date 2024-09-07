#include "./../../include/core/data.h"
#include <cmath>
#include <iostream>
#include <memory>

#define SetBit(Arr, b) (((Arr)[(b) / 8]) |= (1 << ((b) % 8)))
#define ClrBit(Arr, b) (((Arr)[(b) / 8]) &= ~(1 << ((b) % 8)))
#define TstBit(Arr, b) (((Arr)[(b) / 8]) & (1 << ((b) % 8)))
/// returns the bit at the given position as a char containing '0' or '1'
#define GetBit(var, bit) (((var) >> (bit)) & 1)
#define GetBitReversed(var, bit) (var & (1 << (7 - bit)))
#define GetBitAsChar(var, bit) ((GetBit(var, bit)) ? '1' : '0')
/// returns the bit at the given position reversed from the current endianess
#define GetBitAsCharReversed(var, bit) ((GetBitReversed(var, bit)) ? '1' : '0')


int getNumberParityBits(int numberDataBits)
{
    for (int i = 0; i < numberDataBits; i++)
    {
        if ((1 << i) > numberDataBits)
            return i;
    }
    return 0;
}

int sizeOfHammingArrayInBytes(int numberDataBits)
{
    return (getNumberParityBits(numberDataBits) + numberDataBits + 7) >> 3; // +7 damit bei ganzzahl div das nächste Byte nicht verloren geht
}

int isParityBit(int pos)
{
    // 0001 -> 1 -> 1 & 0 = 0001 & 0000 == 0
    // 0010 -> 2 -> 2 & 1 = 0010 & 0001 == 0
    // 0011 -> 3 -> 3 & 2 = 0011 & 0010 == 1
    // 0100 -> 4 -> 4 & 3 = 0100 & 0011 == 0
    // 0101 -> 5 -> 5 & 4 = 0101 & 0100 == 1
    // 0110 -> 6 -> 6 & 5 = 0110 & 0101 == 1
    // 0111 -> 7 -> 7 & 6 = 0111 & 0110 == 1
    // 1000 -> 8 -> 8 & 7 = 1000 & 0111 == 0
    return (pos & (pos - 1)) == 0;
}
int checkParity(const char *hammingArray, int numberParityBits, int parityPos)
{
    // count % 2 != 0
    int parity = 0;
    for (int i = parityPos; i <= numberParityBits; i++)
    {
        if (i & parityPos)
            if (TstBit(hammingArray, i - 1))
                parity = !parity;
    }
    return parity;
}

int calcParityBitsFromData(int dataBits) {
    int parityBits = 0;

    while (std::pow(2, parityBits) < (parityBits + dataBits + 1)) {
        parityBits++;
    }

    return parityBits;
}

int calcParityBitsFromTotal(int totalBits) {
    int parityBits = 0;

    while (std::pow(2, parityBits) < totalBits + 1) {
        parityBits++;
    }

    return parityBits;
}
class Hamming : public Data {
    public:
        Hamming(unsigned int elementLength) : Data(elementLength, elementLength - getNumberParityBits(elementLength)) {

        }
        ~Hamming(){}
        int checkData(Array* data){
            if (data == nullptr)
                return -1;
            return checkAndCorrectData(data);
        }
        std::unique_ptr<Array> getData(Array* encodedData){
            if (encodedData == nullptr)
                return nullptr;
            return readData(encodedData);
        }
        std::unique_ptr<Array> encodeData(Array* data){
            if (data == nullptr)
                return nullptr;
            if (data->getLength() > getDataLength()){
                return nullptr;
            }
            return writeHammingCode(data);
        }

    private:
        int checkAndCorrectData(Array* data) {
            const int numberParityBits = getNumberParityBits(data->getLength());
            unsigned int errorPos = 0;
            bool wasChanged = false;

            for (int i = 0; i < numberParityBits; i++)
            {
                const int parityPos = (1 << i);
                if (checkParity((const char*) data->getArray(), numberParityBits, parityPos)) {
                    wasChanged = true;
                    errorPos += parityPos;
                }
            }
            if (!wasChanged)
                return 0; // No error
            if (errorPos != 0 && errorPos <= data->getLength())
            {
                *SysOut() << "Error at position: " << errorPos << std::endl;
                if (TstBit((const char*) data->getArray(), errorPos - 1))
                    ClrBit(data->getArray(), errorPos - 1);
                else
                    SetBit(data->getArray(), errorPos - 1);
                return 1; // Corrected
            }
            return -1; // Error not correctable
        }

        std::unique_ptr<Array> readData(Array* encodedData) {
            const int numberParityBits = calcParityBitsFromTotal(encodedData->getLength());
            const int numberDataBits = encodedData->getLength() - numberParityBits;

            auto output = std::make_unique<Array>(numberDataBits);

            int j = 0;
            for (unsigned int i = 1; i <= encodedData->getLength(); i++)
            {
                if (!isParityBit(i))
                {
                    if (TstBit(encodedData->getArray(), i - 1))
                        SetBit(output->getArray(), j);
                    else
                        ClrBit(output->getArray(), j);
                    j++;
                }
            }
            return output;
        }

        std::unique_ptr<Array> writeHammingCode(Array* data) {
            int numberDataBits = data->getLength();
            auto hammingArray = std::make_unique<Array>(sizeOfHammingArrayInBytes(numberDataBits));

            const int numberParityBits = getNumberParityBits(numberDataBits);
            const int totalBits = numberDataBits + numberParityBits;
            int curParityBit = 0;
            for (int i = 0; i < totalBits; i++)
            {
                if (isParityBit(i + 1))
                {
                    ClrBit(hammingArray->getArray(), i);
                    curParityBit++;
                }
                else if (TstBit(data->getArray(), i - curParityBit))
                {
                    SetBit(hammingArray->getArray(), i);
                }
            }
            for (int p = 0; p < numberParityBits; p++)
            {
                int parityBitPos = (1 << p);
                if (checkParity((const char*) hammingArray->getArray(), numberParityBits, parityBitPos))
                    SetBit(hammingArray->getArray(), parityBitPos - 1);
                else
                    ClrBit(hammingArray->getArray(), parityBitPos - 1);
            }
            return hammingArray;
        }
};
