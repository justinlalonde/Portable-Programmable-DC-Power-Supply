#include "state_machine.h"

static pcb_state_t current_state = CV;
static pcb_flags_t current_flags;

static float voltage_target = 5.0f, current_target = 1.0f, tempValue;
static int OC_buffer = 0; 
static int OO_streak, SC_streak = 0; 
static int OC_last = OC_LAST_TICKS; 
static int ES_last = ES_LAST_TICKS;
static int start_timer = 0;

static void handle_oo(void)
{
    ledGreenOff();
    ledRedOff();
    
    updateTargets(&voltage_target, &current_target);
    setOutputValues(voltage_target, current_target);
    outputOn();

    setDisplayValue(voltage_target, 'v');
    setDisplayValue(current_target, 'a');
    setDisplayValue(voltage_target*current_target, 'w');
    if(start_timer >= START_TIMER_TICKS){
        if(OC_buffer==0 && ES_last >= ES_LAST_TICKS) displayOn('b');
        else
        {
            displayOn('v');
            displayOn('a');
            displayOff('w');
        }
    }
}
static void handle_cv(void)
{
    if(start_timer >= START_TIMER_TICKS){
        ledGreenOn();
        ledRedOff();
    }
    
    updateTargets(&voltage_target, &current_target);
    setOutputValues(voltage_target, current_target);
    outputOn();

    setDisplayValue(getOutputVoltage(), 'v');
    setDisplayValue(current_target, 'a');
    if(start_timer >= START_TIMER_TICKS){
        displayOn('v');
        displayOn('a');
        displayOff('w');
    }
}
static void handle_cc(void)
{
    if(start_timer >= START_TIMER_TICKS){
        ledGreenOff();
        ledRedOn();
    }
    
    updateTargets(&tempValue, &current_target);
    setOutputValues(voltage_target, current_target);
    outputOn();

    setDisplayValue(getOutputVoltage(), 'v');
    setDisplayValue(current_target, 'a');
    if(start_timer >= START_TIMER_TICKS){
        displayOn('v');
        displayOn('a');
        displayOff('w');
    }
}
static void handle_es(void)
{
    ledGreenOff();
    ledRedOff();

    updateTargets(&tempValue, &current_target);
    setOutputValues(voltage_target, current_target);
    outputOff();

    setDisplayValue(getOutputVoltage(), 'v');
    setDisplayValue(current_target, 'a');
    displayErrTag('w');
    displayOn('b');
}
static void handle_sc(void)
{
    if(start_timer >= START_TIMER_TICKS){
        ledGreenOff();
        ledRedOn();
    }

    outputOn();
    setOutputValues(voltage_target, current_target);

    setDisplayValue(getOutputVoltage(), 'v');
    setDisplayValue(current_target, 'a');
    if(start_timer >= START_TIMER_TICKS){
        displaySCTag('w');
        displayOn('b');
    }
}
static void update_flags(void)
{
    current_flags.SC = !getNFAULTStatus();
    current_flags.OC = getOCPStatus();
    current_flags.OO = !getAUXOFFStatus();
    current_flags.ES = current_target <= CURRENT_LOW_THRSH;
}
static void setCounters()
{
    if(current_flags.ES) 
    {
        OC_buffer = OC_last = ES_last = OO_streak = SC_streak = 0;
        return;
    }
    else if (++ES_last >= ES_LAST_TICKS) ES_last = ES_LAST_TICKS;

    if(current_flags.OC) 
    {
        if(OC_buffer++ >= OC_BUFFER_TICKS)
            if(OC_buffer >= 2*OC_BUFFER_TICKS)
                OC_buffer = 2*OC_BUFFER_TICKS;
    }
    else if (--OC_buffer < 0) OC_buffer = 0;

    if(current_flags.OC) OC_last = 0;
    else if (++OC_last >= OC_LAST_TICKS) OC_last = OC_LAST_TICKS;

    if (++start_timer >= START_TIMER_TICKS) start_timer = START_TIMER_TICKS;

    if(current_flags.OO)
    {
        if(++OO_streak >= OO_STREAK_TICKS)
            OO_streak = OO_STREAK_TICKS;
    }
    else OO_streak=0;

    if(current_flags.SC)
    {
        if(++SC_streak >= SC_STREAK_TICKS)
            SC_streak = SC_STREAK_TICKS;
    }
    else SC_streak=0;
}
static pcb_state_t get_next_state()
{
    if(current_flags.ES)
        return ES;
    
    if ((((OO_streak >= OO_STREAK_TICKS) && (OC_last < OC_LAST_TICKS)) ||
            ((SC_streak >= SC_STREAK_TICKS) && (OC_last < OC_LAST_TICKS))) &&
            (ES_last >= ES_LAST_TICKS))
        return SC;

    if(OC_buffer >= OC_BUFFER_TICKS) 
        return CC;

    if(current_flags.OO && ES_last >= 2) 
        return OO;

    return CV;
}
esp_err_t state_machine_step(void)
{
    update_flags();
    setCounters();
    current_state = get_next_state();
    //logFlags();
    
    switch (current_state)
    {
        case OO:    handle_oo();    break;
        case CV:    handle_cv();    break;
        case CC:    handle_cc();    break;
        case ES:    handle_es();    break;
        case SC:    handle_sc();    break;
        default:    return ESP_ERR_INVALID_ARG;
    }
    return ESP_OK;
}
