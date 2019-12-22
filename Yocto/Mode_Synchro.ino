//=============================================================================
// All interrupt initializations for the three modes are here.

void Mode_Synchro(byte mode)
{
    switch (mode) {

    case 0:
        sync_mode = MASTER;
        cli();//arrete toutes les interruptions
        //UCSR1B &= ~(1 << RXCIE1); //disable UART1 interrupt
        PCMSK3 = 0x00;//disable l'interruption par PIN Change
        Timer1.attachInterrupt(Count_96PPQN); // attache l'interruption timer Master
        Timer3.attachInterrupt(Count_Clock);
        DDRD |= ((1 << 4) | (1 << 5)); //Met les pins DIN start et clock en sorti
        sei();                     // turn on interrupts
        break;

    case 1:
        sync_mode = DIN_SLAVE;
        cli();//arrete toutes les interruptions
        //UCSR1B &= ~(1 << RXCIE1); //disable UART1 interrupt
        Timer1.detachInterrupt();
        Timer3.detachInterrupt();
        DDRD &= ~((1 << 4) | (1 << 5)) ; //Pin DIN CLK et DIN Start en entree
        PCICR |= (1 << PCIE3);     // set PCIE3 to enable PCMSK3 scan
        PCMSK3 |= ((1 << PCINT28) | (1 << PCINT29)); // set PCINT28 et PCINT29 to trigger an interrupt on state change
        sei();                     // turn on interrupts
        break;

    case 2:
        sync_mode = MIDI_SLAVE;
        cli();//arrete toutes les interruptions
        PCMSK3 = 0x00;//disable l'interruption par PIN Change
        Timer1.detachInterrupt();
        Timer3.detachInterrupt();
        Timer3.attachInterrupt(Count_Clock);
        //UCSR1B |= (1 << RXCIE1); // Enable the USART1 Recieve Complete interrupt (USART1_RX)
        DDRD |= ((1 << 4) | (1 << 5)) ; //Pin DIN CLK et DIN Start en sorti
        sei(); // Enable the Global Interrupt Enable flag so that interrupts can be processed
        break;
    }
}
