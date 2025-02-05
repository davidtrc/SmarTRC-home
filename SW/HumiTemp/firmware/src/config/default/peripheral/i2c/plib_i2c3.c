/*******************************************************************************
  Inter-Integrated Circuit (I2C) Library
  Source File

  Company:
    Microchip Technology Inc.

  File Name:
    plib_i2c3.c

  Summary:
    I2C PLIB Implementation file

  Description:
    This file defines the interface to the I2C peripheral library.
    This library provides access to and control of the associated peripheral
    instance.

*******************************************************************************/
// DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2018-2019 Microchip Technology Inc. and its subsidiaries.
*
* Subject to your compliance with these terms, you may use Microchip software
* and any derivatives exclusively with Microchip products. It is your
* responsibility to comply with third party license terms applicable to your
* use of third party software (including open source software) that may
* accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
* EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
* WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
* PARTICULAR PURPOSE.
*
* IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
* INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
* WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
* BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
* ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*******************************************************************************/
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "device.h"
#include "plib_i2c3.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global Data
// *****************************************************************************
// *****************************************************************************

static I2C_OBJ i2c3Obj;

void I2C3_Initialize(void)
{
    /* Disable the I2C Master interrupt */
    IEC5CLR = _IEC5_I2C3MIE_MASK;

    /* Disable the I2C Bus collision interrupt */
    IEC5CLR = _IEC5_I2C3BIE_MASK;

    I2C3BRG = 993;

    I2C3CONCLR = _I2C3CON_SIDL_MASK;
    I2C3CONCLR = _I2C3CON_DISSLW_MASK;
    I2C3CONCLR = _I2C3CON_SMEN_MASK;

    /* Initialize master interrupt */
    IFS5CLR = _IFS5_I2C3MIF_MASK;
    IEC5SET = _IEC5_I2C3MIE_MASK;

    /* Initialize fault interrupt */
    IFS5CLR = _IFS5_I2C3BIF_MASK;
    IEC5SET = _IEC5_I2C3BIE_MASK;

    /* Turn on the I2C module */
    I2C3CONSET = _I2C3CON_ON_MASK;

    /* Set the initial state of the I2C state machine */
    i2c3Obj.state = I2C_STATE_IDLE;
}

/* I2C state machine */
static void I2C3_TransferSM(void)
{
    switch (i2c3Obj.state)
    {
        case I2C_STATE_START_CONDITION:
            /* Generate Start Condition */
            I2C3CONSET = _I2C3CON_SEN_MASK;
            i2c3Obj.state = I2C_STATE_ADDR_BYTE_1_SEND;
            break;
        case I2C_STATE_ADDR_BYTE_1_SEND:
            /* Is transmit buffer full? */
            if (!(I2C3STAT & _I2C3STAT_TBF_MASK))
            {
                if (i2c3Obj.address > 0x007F)
                {
                    /* Transmit the first byte of the 10-bit slave address */
                    I2C3TRN = ( 0xF0 | (((uint8_t*)&i2c3Obj.address)[1] << 1) | (i2c3Obj.transferType) );

                    if (i2c3Obj.transferType == I2C_TRANSFER_TYPE_WRITE)
                    {
                        i2c3Obj.state = I2C_STATE_ADDR_BYTE_2_SEND;
                    }
                    else
                    {
                        i2c3Obj.state = I2C_STATE_READ;
                    }
                }
                else
                {
                    /* 8-bit addressing mode */
                    I2C3TRN = ((i2c3Obj.address << 1) | i2c3Obj.transferType);

                    if (i2c3Obj.transferType == I2C_TRANSFER_TYPE_WRITE)
                    {
                        i2c3Obj.state = I2C_STATE_WRITE;
                    }
                    else
                    {
                        i2c3Obj.state = I2C_STATE_READ;
                    }
                }
            }
            break;
        case I2C_STATE_ADDR_BYTE_2_SEND:
            /* Transmit the 2nd byte of the 10-bit slave address */
            if (!(I2C3STAT & _I2C3STAT_ACKSTAT_MASK))
            {
                if (!(I2C3STAT & _I2C3STAT_TBF_MASK))
                {
                    I2C3TRN = i2c3Obj.address;
                    i2c3Obj.state = I2C_STATE_WRITE;
                }
            }
            else
            {
                /* NAK received. Generate Stop Condition. */
                i2c3Obj.error = I2C_ERROR_NACK;
                I2C3CONSET = _I2C3CON_PEN_MASK;
                i2c3Obj.state = I2C_STATE_WAIT_STOP_CONDITION_COMPLETE;
            }
            break;
        case I2C_STATE_WRITE:
            if (!(I2C3STAT & _I2C3STAT_ACKSTAT_MASK))
            {
                /* ACK received */
                if (i2c3Obj.writeCount < i2c3Obj.writeSize)
                {
                    if (!(I2C3STAT & _I2C3STAT_TBF_MASK))
                    {
                        /* Transmit the data from writeBuffer[] */
                        I2C3TRN = i2c3Obj.writeBuffer[i2c3Obj.writeCount++];
                    }
                }
                else
                {
                    if (i2c3Obj.readCount < i2c3Obj.readSize)
                    {
                        /* Generate repeated start condition */
                        I2C3CONSET = _I2C3CON_RSEN_MASK;
                        i2c3Obj.transferType = I2C_TRANSFER_TYPE_READ;
                        /* Send the I2C slave address with R/W = 1*/
                        i2c3Obj.state = I2C_STATE_ADDR_BYTE_1_SEND;
                    }
                    else
                    {
                        /* Transfer Complete. Generate Stop Condition */
                        I2C3CONSET = _I2C3CON_PEN_MASK;
                        i2c3Obj.state = I2C_STATE_WAIT_STOP_CONDITION_COMPLETE;
                    }
                }
            }
            else
            {
                /* NAK received. Generate Stop Condition. */
                i2c3Obj.error = I2C_ERROR_NACK;
                I2C3CONSET = _I2C3CON_PEN_MASK;
                i2c3Obj.state = I2C_STATE_WAIT_STOP_CONDITION_COMPLETE;
            }
            break;
        case I2C_STATE_READ:
            if (!(I2C3STAT & _I2C3STAT_ACKSTAT_MASK))
            {
                /* Slave ACK'd the device address. Enable receiver. */
                I2C3CONSET = _I2C3CON_RCEN_MASK;
                i2c3Obj.state = I2C_STATE_READ_BYTE;
            }
            else
            {
                /* NAK received. Generate Stop Condition. */
                i2c3Obj.error = I2C_ERROR_NACK;
                I2C3CONSET = _I2C3CON_PEN_MASK;
                i2c3Obj.state = I2C_STATE_WAIT_STOP_CONDITION_COMPLETE;
            }
            break;

        case I2C_STATE_READ_BYTE:
            /* Data received from the slave */
            if (I2C3STAT & _I2C3STAT_RBF_MASK)
            {
                i2c3Obj.readBuffer[i2c3Obj.readCount++] = I2C3RCV;
                if (i2c3Obj.readCount == i2c3Obj.readSize)
                {
                    /* Send NAK */
                    I2C3CONSET = _I2C3CON_ACKDT_MASK;
                    I2C3CONSET = _I2C3CON_ACKEN_MASK;
                }
                else
                {
                    /* Send ACK */
                    I2C3CONCLR = _I2C3CON_ACKDT_MASK;
                    I2C3CONSET = _I2C3CON_ACKEN_MASK;
                }
                i2c3Obj.state = I2C_STATE_WAIT_ACK_COMPLETE;
            }
            break;

        case I2C_STATE_WAIT_ACK_COMPLETE:
            /* ACK or NAK sent to the I2C slave */
            if (i2c3Obj.readCount < i2c3Obj.readSize)
            {
                /* Enable receiver */
                I2C3CONSET = _I2C3CON_RCEN_MASK;
                i2c3Obj.state = I2C_STATE_READ_BYTE;
            }
            else
            {
                /* Generate Stop Condition */
                I2C3CONSET = _I2C3CON_PEN_MASK;
                i2c3Obj.state = I2C_STATE_WAIT_STOP_CONDITION_COMPLETE;
            }
            break;

        case I2C_STATE_WAIT_STOP_CONDITION_COMPLETE:
            i2c3Obj.state = I2C_STATE_IDLE;
            if (i2c3Obj.callback != NULL)
            {
                i2c3Obj.callback(i2c3Obj.context);
            }
            break;

        default:
            break;
    }
    IFS5CLR = _IFS5_I2C3MIF_MASK;
}


void I2C3_CallbackRegister(I2C_CALLBACK callback, uintptr_t contextHandle)
{
    if (callback == NULL)
    {
        return;
    }

    i2c3Obj.callback = callback;
    i2c3Obj.context = contextHandle;
}

bool I2C3_IsBusy(void)
{
    if( (i2c3Obj.state != I2C_STATE_IDLE ) || (I2C3CON & 0x0000001F) ||
        (I2C3STAT & _I2C3STAT_TRSTAT_MASK) || (I2C3STAT & _I2C3STAT_S_MASK) )
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool I2C3_Read(uint16_t address, uint8_t* rdata, size_t rlength)
{
    /* State machine must be idle and I2C module should not have detected a start bit on the bus */
    if((i2c3Obj.state != I2C_STATE_IDLE) || (I2C3STAT & _I2C3STAT_S_MASK))
    {
        return false;
    }

    i2c3Obj.address             = address;
    i2c3Obj.readBuffer          = rdata;
    i2c3Obj.readSize            = rlength;
    i2c3Obj.writeBuffer         = NULL;
    i2c3Obj.writeSize           = 0;
    i2c3Obj.writeCount          = 0;
    i2c3Obj.readCount           = 0;
    i2c3Obj.transferType        = I2C_TRANSFER_TYPE_READ;
    i2c3Obj.error               = I2C_ERROR_NONE;
    i2c3Obj.state               = I2C_STATE_START_CONDITION;

    I2C3_TransferSM();

    return true;
}


bool I2C3_Write(uint16_t address, uint8_t* wdata, size_t wlength)
{
    /* State machine must be idle and I2C module should not have detected a start bit on the bus */
    if((i2c3Obj.state != I2C_STATE_IDLE) || (I2C3STAT & _I2C3STAT_S_MASK))
    {
        return false;
    }

    i2c3Obj.address             = address;
    i2c3Obj.readBuffer          = NULL;
    i2c3Obj.readSize            = 0;
    i2c3Obj.writeBuffer         = wdata;
    i2c3Obj.writeSize           = wlength;
    i2c3Obj.writeCount          = 0;
    i2c3Obj.readCount           = 0;
    i2c3Obj.transferType        = I2C_TRANSFER_TYPE_WRITE;
    i2c3Obj.error               = I2C_ERROR_NONE;
    i2c3Obj.state               = I2C_STATE_START_CONDITION;

    I2C3_TransferSM();

    return true;
}

bool I2C3_WriteRead(uint16_t address, uint8_t* wdata, size_t wlength, uint8_t* rdata, size_t rlength)
{
    /* State machine must be idle and I2C module should not have detected a start bit on the bus */
    if((i2c3Obj.state != I2C_STATE_IDLE) || (I2C3STAT & _I2C3STAT_S_MASK))
    {
        return false;
    }

    i2c3Obj.address             = address;
    i2c3Obj.readBuffer          = rdata;
    i2c3Obj.readSize            = rlength;
    i2c3Obj.writeBuffer         = wdata;
    i2c3Obj.writeSize           = wlength;
    i2c3Obj.writeCount          = 0;
    i2c3Obj.readCount           = 0;
    i2c3Obj.transferType        = I2C_TRANSFER_TYPE_WRITE;
    i2c3Obj.error               = I2C_ERROR_NONE;
    i2c3Obj.state               = I2C_STATE_START_CONDITION;

    I2C3_TransferSM();

    return true;
}

I2C_ERROR I2C3_ErrorGet(void)
{
    I2C_ERROR error;

    error = i2c3Obj.error;
    i2c3Obj.error = I2C_ERROR_NONE;

    return error;
}

void I2C3_BUS_InterruptHandler(void)
{
    /* Clear the bus collision error status bit */
    I2C3STATCLR = _I2C3STAT_BCL_MASK;

    /* ACK the bus interrupt */
    IFS5CLR = _IFS5_I2C3BIF_MASK;

    i2c3Obj.state = I2C_STATE_IDLE;

    i2c3Obj.error = I2C_ERROR_BUS_COLLISION;

    if (i2c3Obj.callback != NULL)
    {
        i2c3Obj.callback(i2c3Obj.context);
    }
}

void I2C3_MASTER_InterruptHandler(void)
{
    I2C3_TransferSM();
}
