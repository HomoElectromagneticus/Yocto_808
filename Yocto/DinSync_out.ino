// All function related to dinsync handling.

void Set_Dinsync_Clock_High()
{
    PORTD |= (1 << 4); // Set the clock high.
    dinsync_clock_timeout = 4; // 4ms clock.
}

void Set_Dinsync_Clock_Low()
{
    PORTD &= ~(1 << 4); // Lower the clock signal.
}

void Set_Dinsync_Run_High()
{
    PORTD |= (1 << 5); // Set Run = 1.
}

void Set_Dinsync_Run_Low()
{
    PORTD &= ~(1 << 5); // Set Run = 0.
}