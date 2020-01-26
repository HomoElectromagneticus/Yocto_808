//=======================================================================
//function which initializes an empty pattern with 16 steps on a 1/16 scale
//fonction qui initialize un pattern vide avec 16pas et la scale en 1/16
//=======================================================================

void Clear_Pattern()
{

    // check if the edit buttons in setup mode
    Check_Edit_Button_Setup();//check si les boutons edit en mode setup

    // we read the buttons
    unsigned int reading = SR.Button_Step_Read();// on lit les boutons

    // if they have changed, you record the time
    if (reading != old_step_button_state) { // si ils ont changer on enregistre le temps
        millis_debounce_step_button = millis();
    }

    if ((millis() - millis_debounce_step_button) >= DEBOUNCE) {
        if (reading != step_button_state) {
            step_button_state = reading;

            if (!button_shift) {
                // loop as many times as the step button becomes 16
                for (byte i = 0; i < 16; i++) { //loop autant de fois que de bouton step soit 16
                    if (bitRead(step_button_state, i)) {
                        // we return the value of the selected pattern
                        //on retourne la valeur du pattern selectionner
                        selected_pattern = i;
                        // flag for if the selected pattern has changed
                        selected_pattern_changed = 1; //flag que le pattern selectionner a change
                        break;
                    }
                }
            }
            else if (button_shift) {
                // loop as many times as the step button becomes 16
                for (byte i = 0; i < 16; i++) { //loop autant de fois que de bouton step soit 16
                    if (bitRead(step_button_state, i)) {
                        // if the shift button is pressed, you return the selected bank
                        // si le bouton shift est appuyer on retourne la bank selectionner selectionner
                        pattern_bank = i;
                        // flag for if the selected pattern has changed
                        selected_pattern_changed = 1; //flag que le pattern selectionner a change
                        break;
                    }
                }
            }

            // the pattern number is equal to the selected pattern plus 16 times the bank or 255 pattern
            pattern_nbr = selected_pattern + (16 * pattern_bank); //le numero du pattern est egal au pattern selectionner plus 16 fois la bank soit 255 pattern
        }
    }

    old_step_button_state = reading;

    // if the selected pattern changed
    // si le pattern selectionner a changer
    if (old_pattern_nbr != pattern_nbr) {
        Load_Pattern();
        // the selected pattern has not been cleared
        pattern_clear_ok = 0; //le pattern selectionner n'a pas été clearer
    }

    // if one presses the play button and if the pattern has not yet been cleared or has been edited or if we have just returned to the Clear_Pattern mode
    //Si on appuie sur play et que le pattern n'a pas necore etait clearer ou a ete editer ou qu'on vient de rentrer dans le mode Clear_Pattern
    if (button_play && (!pattern_clear_ok || selected_pattern_changed || first_time_clear_pattern)) {
        // initialize the flog for entry for the mode Pattern_Clear
        first_time_clear_pattern = 0; //initialise le flag d'entrée dans le mode pattern clear
        selected_pattern_changed = 0;
        Clear_EEprom_Pattern();
        Chenillard();
        // the selected pattern has been cleared
        pattern_clear_ok = 1; //le pattern selectionner a ete clearer
        pattern_buffer = !pattern_buffer;
        // one reload the cleared pattern
        Load_Pattern();//on recharge le pattern clearer
        //pattern_buffer=!pattern_buffer;
        // the selected pattern is reset
        selected_pattern_changed = 0; //le pattern selectionner est reseter
    }
}

//=======================================================================
//function which copies the selected pattern in the buffer
//fonction qui copie le pattern selectionner dans le buffer
//=======================================================================

void Copy_Pattern()
{

    unsigned int reading = SR.Button_Step_Read();// on lit les boutons

    if (reading != old_step_button_state) { // si ils ont changer on enregistre le temps
        millis_debounce_step_button = millis();
    }

    if ((millis() - millis_debounce_step_button) >= DEBOUNCE) {
        if (reading != step_button_state) {
            step_button_state = reading;

            if (!button_shift) {
                for (byte i = 0; i < 16; i++) { //loop autant de fois que de bouton step soit 16
                    if (bitRead(step_button_state, i)) {
                        //on retourne la valeur du pattern selectionner
                        selected_pattern = i;
                        selected_pattern_changed = 1; //flag que le pattern selectionner a change
                        break;
                    }
                }
            }
            else if (button_shift) {
                for (byte i = 0; i < 16; i++) { //loop autant de fois que de bouton step soit 16
                    if (bitRead(step_button_state, i)) {
                        // si le bouton shift est appuyer on retourne la bank selectionner selectionner
                        pattern_bank = i;
                        selected_pattern_changed = 1; //flag que le pattern selectionner a change
                        break;
                    }
                }
            }

            pattern_nbr = selected_pattern + (16 * pattern_bank); //le numero du pattern est egal au pattern selectionner plus 16 fois la bank soit 255 pattern
        }
    }

    old_step_button_state = reading;



    // si le pattern selectionner a changer
    if (old_pattern_nbr != pattern_nbr) {
        Load_Pattern();
        pattern_copy_ok = 0; //le pattern selectionner n'a pas été copier
    }

    if (button_play && (!pattern_copy_ok || selected_pattern_changed || first_time_copy_pattern)) {

        first_time_copy_pattern = 0; //initialise le flag d'entrée dans le mode pattern clear
        selected_pattern_changed = 0;

        for (char i = 0; i < NBR_INST; i++) { //loop autant de fois que d'instruments
            for (char j = 0; j < 2; j++) { //loop deux fois pour les deux parti du pattern 1a16 et 17 a 32
                // copy_pattern_buffer[i][j] =0;//initialise le buffer
                copy_pattern_buffer[i][j] = pattern[pattern_buffer][i][j];//On met le pattern a copier dans le buffer
            }
        }

        copy_pattern_nbr_step = nbr_step[pattern_buffer]; //copie le nombre de pas dans le buffer
        copy_pattern_scale = pattern_scale[pattern_buffer]; //copie la scale dans le buffer

        //fonction chenillard des leds qui indique que le pattern a ete copier
        Chenillard();
        pattern_copy_ok = 1; //le pattern selectionner a ete copier
        selected_pattern_changed = 0; //initialise que le pattern changer a été copier
    }
}

//=======================================================================
//function which pastes the pattern in the buffer to the selected pattern
//fonction qui paste le pattern dans le buffer dans le pattern selectionne
//=======================================================================

void Paste_Pattern()
{

    unsigned int reading = SR.Button_Step_Read();// on lit les boutons

    if (reading != old_step_button_state) { // si ils ont changer on enregistre le temps
        millis_debounce_step_button = millis();
    }

    if ((millis() - millis_debounce_step_button) >= DEBOUNCE) {
        if (reading != step_button_state) {
            step_button_state = reading;

            if (!button_shift) {
                for (byte i = 0; i < 16; i++) { //loop autant de fois que de bouton step soit 16
                    if (bitRead(step_button_state, i)) {
                        //on retourne la valeur du pattern selectionner
                        selected_pattern = i;
                        selected_pattern_changed = 1; //flag que le pattern selectionner a change
                        break;
                    }
                }
            }
            else if (button_shift) {
                for (byte i = 0; i < 16; i++) { //loop autant de fois que de bouton step soit 16
                    if (bitRead(step_button_state, i)) {
                        // si le bouton shift est appuyer on retourne la bank selectionner selectionner
                        pattern_bank = i;
                        selected_pattern_changed = 1; //flag que le pattern selectionner a change
                        break;
                    }
                }
            }

            pattern_nbr = selected_pattern + (16 * pattern_bank); //le numero du pattern est egal au pattern selectionner plus 16 fois la bank soit 255 pattern
        }
    }

    old_step_button_state = reading;

    if (button_play && (selected_pattern_changed || first_time_paste_pattern)) {
        selected_pattern_changed = false;
        first_time_paste_pattern = 0; //initialise le flag d'entrée dans le mode pattern clear
        Save_Paste_Pattern();
        Chenillard();
    }
}
