#include <Array.h>

#include <stdlib.h>

int ArrayGrow(void** array, int capacity, int elementSize)
{
    int newCapacity = (capacity > 16) ? capacity - 1 : 15;
    newCapacity = newCapacity | (newCapacity >> 1);
    newCapacity = newCapacity | (newCapacity >> 2);
    newCapacity = newCapacity | (newCapacity >> 4);
    newCapacity = newCapacity | (newCapacity >> 8);
    newCapacity = newCapacity | (newCapacity >> 16);
    newCapacity = newCapacity + 1;

    int* oldBuffer = *array ? (int*)*array - 2 : 0;
    int* newBuffer = realloc(oldBuffer, sizeof(int) * 2 + newCapacity * elementSize);

    if (newBuffer)
    {
        newBuffer[0] = oldBuffer ? oldBuffer[0] : 0;
        newBuffer[1] = newCapacity;

        *array = (newBuffer + 2);

        return 1;
    }
    else
    {
        return 0;
    }
}