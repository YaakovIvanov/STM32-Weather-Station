#include "LiquidCrystal.h"

	unsigned char _displaycontrol;
    unsigned char _displayfunction;
    unsigned char _displaymode;
    unsigned char _numlines;
    unsigned char _rowOffsets[4];
    char          numstr[7];

    uint16_t	  dataPins[8];
    uint16_t	  _rs;
    uint16_t	  _rw;
    uint16_t      _enable;
    GPIO_TypeDef  *gpioPort;

    /*
     * Pin setup
     * -------
     * Maps the STM32 pins to the LCD pins
     * This function must be called before any other LCD functions (including begin)
     *
     * The mapping assumes all connections to the LCD pins are made on a single GPIO port
     * (GPIOx, where x = A, B, C, etc.)
     * and that the pins are of the form GPIO_PIN_x (where x = 0, 1, 2, etc.)
     *
     * The function assumes all GPIO ports have already been initialized prior by the user
     */
    void pin_setup(GPIO_TypeDef *GPIOx, uint16_t d0, uint16_t d1, uint16_t d2, uint16_t d3,
    		uint16_t d4, uint16_t d5, uint16_t d6, uint16_t d7, uint16_t rs, uint16_t rw, uint16_t e)
    {
        // GPIO port
    	gpioPort = GPIOx;

    	// LCD data signals are now on dataPins
        dataPins[0] = d0;
        dataPins[1] = d1;
        dataPins[2] = d2;
        dataPins[3] = d3;
        dataPins[4] = d4;
        dataPins[5] = d5;
        dataPins[6] = d6;
        dataPins[7] = d7;

        // LCD control signals are now on _rs, _rw, and _enable
        _rs = rs;
        _rw = rw;
        _enable = e;

    }

    /*
     * begin
     * ------
     * Initializes the LCD and specifies the dimensions (width and hight) of the display
     * This function must be called after pin_setup and before any other LCD function
     *
     * cols: Number of columns (mine is 16)
     *
     * line: Number of lines (mine is 2)
     *
     * dotsize: can be either LCD_5x8DOTS or LCD_5x10DOTS (mine is 5x8)
     */
    void begin(unsigned char cols, unsigned char lines, unsigned char dotsize)
    {
        // Display specifications
        // Default: 8 bit mode, 1 line, 5x8 dots= size

        _displayfunction = LCD_8BITMODE | LCD_1LINE | LCD_5x8DOTS;

         if (lines > 1)
        {
            _displayfunction |= LCD_2LINE;
        }

        _numlines = lines; // store the number of lines

        // Figure 4 in the datasheet (page 11)
        // First row address in DDRAM starts at 0x00
        // Second row address in DDRAM start at 0x40
        // Third row (left of first row) address in DDRAM starts at 0x00 + cols
        // Fourth row (left of second row) address in DDRAM starts at 0x40 + cols)
        setRowOffsets(0x00, 0x40, 0x00 + cols, 0x40 + cols);

        if (dotsize != LCD_5x8DOTS)
            _displayfunction |= LCD_5x10DOTS;

        // Initialize LCD
        // SEE PAGE 45/46 for initialization specifications
        // We will just focus on 8 bits mode
        HAL_Delay(41); // We need at least 40 ms after power rises above 2.7 V
        sendCommand(LCD_FUNCTIONSET | _displayfunction);
        HAL_Delay(5); // wait for 4.1 ms
        sendCommand(LCD_FUNCTIONSET | _displayfunction);
        HAL_Delay(1); // wait for more than 100 us
        sendCommand(LCD_FUNCTIONSET | _displayfunction);

        // When the display powers up, it is configured as follows:
        //
        // 1. Display clear
        // 2. Function set:
        //    DL = 1; 8-bit interface data
        //    N = 0; 1-line display
        //    F = 0; 5x8 dot character font
        // 3. Display on/off control:
        //    D = 0; Display off
        //    C = 0; Cursor off
        //    B = 0; Blinking off
        // 4. Entry mode set:
        //    I/D = 1; Increment by 1
        //    S = 0; No shift

        // Now the LCD is on, we configure the lines and dotsize
        sendCommand(LCD_FUNCTIONSET | _displayfunction);

        // turn the display on with no cursor or blinking default

        _displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
        displayON();

        // clear it off
        clear();

        // Initialize default text direction to left to right
        _displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
        sendCommand(LCD_ENTRYMODESET | _displaymode);

    }

    /* write
     * -------
     * Write a character to the LCD
     */
    void write(unsigned char chr)
    {
        sendData(chr);
    }

    /* print
     * ------
     * Print a string
     *
     */
    void print(unsigned char *chr)
    {
        while (*chr != '\0')
        {
            write(*chr);
            chr++;
        }
    }

    /* print_int
     * ------
     * Print an integer
     *
     */
    void print_int(int num)
    {
        sprintf(numstr, "%d", num);
        print(numstr);
    }

    /* displayON
     * ------------
     * Turn on the display
     *
     */
    void displayON()
    {
        _displaycontrol |= LCD_DISPLAYON;
        sendCommand(LCD_DISPLAYCONTROL | _displaycontrol);
    }

    /* display
     * -----------
     * The same as displayON
     * This function is included here to match that found in the
     * Arduino LiquidCrystal library
     *
     */

    void display()
    {
        displayON();
    }

     /* displayOFF
     * ------------
     * Turn off the display
     *
     */

    void displayOFF()
    {
        _displaycontrol &= ~LCD_DISPLAYON;
        sendCommand(LCD_DISPLAYCONTROL | _displaycontrol);
    }

    /* noDisplay
     * -----------
     * The same as displayOFF
     * This function is included here to match that found in the
     * Arduino LiquidCrystal library
     *
     */
    void noDisplay()
    {
        displayOFF();
    }

    /* clear
     * -------
     * Clears the screen and return cursor to home
     */
    void clear()
    {
        sendCommand(LCD_CLEARDISPLAY); // clear display, set cursor position to zero
        HAL_Delay(2); // this command takes a long time :)
    }

    /* home
     * --------
     * Return cursor position to home (upper left for left to right languages)
     * Doesn't clear the screen
     *
     */
    void home()
    {
        sendCommand(LCD_RETURNHOME);
        HAL_Delay(2);
    }

    /*
     * setRowOffsets
     * ---------------
     * Split the 80x8 DDRAM memory into 4 rows and store the row offsets
     *
     * Pages 10, 11 of the datasheet show some figures
     * A rows is not the same as a line, a row is contained within a line
     * For example, in a 16x2 LCD, you have 2 lines, each line can contain
     * two rows each is 16 characters.
     *
     * row0: starting address of first row in the first line
     * row1: starting address of first row in the second line
     * row2: starting address of second row in the first line
     * row3: starting address of second row in the second line

     */
    void setRowOffsets(unsigned char row0, unsigned char row1, unsigned char row2, unsigned char row3)
    {
        _rowOffsets[0] = row0;
        _rowOffsets[1] = row1;
        _rowOffsets[2] = row2;
        _rowOffsets[3] = row3;

    }

    /*
     * setCursor
     * ----------
     * set cursor location
     *
     * col: column number (in my case 0 to 31), the col after 15 are not visible (right of screen)
     * row: row number (in my case 0 to 3)
     */
    void setCursor(unsigned char col, unsigned row)
    {

        // if the row is not within range, assume it is on the last line
        if (row >= _numlines)
            row = _numlines - 1;

        sendCommand(LCD_SETDDRAMADDR | (col + _rowOffsets[row]));
    }


    /*cursor
     *----------
     * Display the cursor as an underline
     */
    void cursor()
    {
        _displaycontrol |= LCD_CURSORON;
        sendCommand(LCD_DISPLAYCONTROL | _displaycontrol);

    }

    /*noCursor
     *----------
     * Display the cursor as an underline
     */
    void noCursor()
    {
        _displaycontrol &= ~LCD_CURSORON;
        sendCommand(LCD_DISPLAYCONTROL | _displaycontrol);

    }

    /* blink
     *----------
     * blink on the blinking cursor
     */
    void blink()
    {
        _displaycontrol |= LCD_BLINKON;
        sendCommand(LCD_DISPLAYCONTROL | _displaycontrol);

    }

    /*noBlink
     *----------
     * turn off the blinking cursor
     */
    void noBlink()
    {
        _displaycontrol &= ~LCD_BLINKON;
        sendCommand(LCD_DISPLAYCONTROL | _displaycontrol);

    }

    /* scrollDisplayLeft
     * -------------------
     * Scrolls the contents of the display (text and cursor) one space to the left)
     */
    void scrollDisplayLeft()
    {
      sendCommand(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
    }

    /* scrollDisplayRight
     * Scrolls the contents of the display (text and cursor) one space to the right
     */
    void scrollDisplayRight()
    {
      sendCommand(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
    }

    /* leftToRight
     * -------------
     * Set the direction for text written to the LCD to left-to-right,
     * the default. This means that subsequent characters written to the
     * display will go from left to right, but does not affect previously-output
     * text.
     */
    void leftToRight()
    {
      _displaymode |= LCD_ENTRYLEFT;
      sendCommand(LCD_ENTRYMODESET | _displaymode);
    }

    /* rightToLeft
     * -------------
     * Set the direction for text written to the LCD to right-to-left
     * (the default is left-to-right). This means that subsequent characters
     *  written to the display will go from right to left, but does not affect
     *  previously-output text.
     */
    void rightToLeft()
    {
      _displaymode &= ~LCD_ENTRYLEFT;
      sendCommand(LCD_ENTRYMODESET | _displaymode);
    }

    /* autoscroll
     * ------------------
     * Turns on automatic scrolling of the LCD
     *
     */
    void autoscroll(void)
    {
      _displaymode |= LCD_ENTRYSHIFTINCREMENT;
      sendCommand(LCD_ENTRYMODESET | _displaymode);
    }

    /* noAutoscroll
     * ------------------
     * Turns off automatic scrolling of the LCD
     */
    void noAutoscroll()
    {
      _displaymode &= ~LCD_ENTRYSHIFTINCREMENT;
      sendCommand(LCD_ENTRYMODESET | _displaymode);
    }

    //------------------------------------------------------------------
    //                    Lower level functions
    //------------------------------------------------------------------

    /*
     * send
     * -------------
     * send Data or command to the LCD
     * the function will take care of setting the correct values for rs and rw
     *
     * value: the 8 bits data value to be sent to the LCD driver
     * mode: 0 = command, 1 = data
     */
    void send(unsigned char value, unsigned char mode)
    {
        if (mode)
        	HAL_GPIO_WritePin(gpioPort, _rs, GPIO_PIN_SET);
        else
        	HAL_GPIO_WritePin(gpioPort, _rs, GPIO_PIN_RESET);


        HAL_GPIO_WritePin(gpioPort, _rw, GPIO_PIN_RESET);

        // use the 8 bit mode (the only one for now)
        if (_displayfunction & LCD_8BITMODE)
        {
            write8bits(value);
        }
    }


    /*
     * sendData
     * -------------
     * send Data to the LCD
     * The function will take care of setting the correct values for rs and rw
     *
     * value: the 8 bits data value to be sent to the LCD driver
     *
     */
    void sendData(unsigned char value)
    {
        send(value, 1);
    }

     /*
     * sendCommand
     * -------------
     * send Command to the LCD
     * The function will take care of setting the correct values for rs and rw
     *
     * value: the 8 bits command value to be sent to the LCD driver
     *
     */
    void sendCommand(unsigned char value)
    {
        send(value, 0);
    }


    /* pulseEnable
     * ----------------
     * create a short tick (1 ms) on the enable pin
     *
     * This function is used to issue an enable tick whenever you need to write
     * or read the value on the data pins
     */
    void pulseEnable(void)
    {
        HAL_GPIO_WritePin(gpioPort, _enable, GPIO_PIN_RESET);
        HAL_Delay(1);
        HAL_GPIO_WritePin(gpioPort, _enable, GPIO_PIN_SET);
        HAL_Delay(1);
        HAL_GPIO_WritePin(gpioPort, _enable, GPIO_PIN_RESET);
        HAL_Delay(1);

    }
    /*
     * write8bits
     * --------------
     * Place the 8 bits of value on the corresponding data pins
     * This function assumes an 8 bit mode
     *
     */
    void write8bits(unsigned char value)
    {
        for (int i = 0; i < 8; i++)
        {
        	HAL_GPIO_WritePin(gpioPort, dataPins[i], ((value >> i) & 0x0001)? GPIO_PIN_SET:GPIO_PIN_RESET);
        }

        pulseEnable();
    }

