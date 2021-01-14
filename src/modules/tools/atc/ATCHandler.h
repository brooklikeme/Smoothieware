#ifndef _ATCHANDLER_H
#define _ATCHANDLER_H

using namespace std;
#include "Module.h"
#include <vector>
#include <map>
#include <queue>
#include "Pin.h"

class StreamOutput;

class ATCHandler : public Module
{
public:
    ATCHandler();

    void on_module_loaded();
    void on_gcode_received(void *argument);
    void on_console_line_received( void *argument );
    void on_second_tick(void *);
    void on_main_loop( void* argument );
    void on_halt(void *argument);
    void on_config_reload(void *argument);


private:
    typedef enum {
        NONE,
        TEST,
        STEP
    } EXTRACTOR_STATUS;

    EXTRACTOR_STATUS extractor_status;

    void set_inner_playing(bool inner_playing);
    bool get_inner_playing() const;

    void load_tests();

    // send data to HMI screen
    void hmi_load_tests(string parameters, StreamOutput *stream); //
    void hmi_load_test_info(string parameters, StreamOutput *stream); //
    // update data to HMI screen
    void hmi_update_test(string parameters, StreamOutput *stream);
    void hmi_update_step(string parameters, StreamOutput *stream);
	// execute command from HMI screen
    void hmi_home(string parameters, StreamOutput *stream);
	void hmi_test(string parameters, StreamOutput *stream);
	void hmi_step(string parameters, StreamOutput *stream);
	void hmi_pause(string parameters, StreamOutput *stream);
	void hmi_stop(string parameters, StreamOutput *stream);

    void fill_test_scripts();
    void fill_step_scripts(int index, int minutes, int pressure);

    void clear_script_queue();

    void rapid_move(float x, float y, float z);

    std::queue<string> script_queue;

    float safe_z;
    float z_rate_work;
    float x_interval;

    float z_pos_work;
    float x_pos_origin;
    float y_pos_waste;
    float y_pos_gather;

    struct reagent {
    	string name;
    	int minutes;	// 1 - 100
    	int pressure;	// 10 - 100
    };

    map<string, vector<struct reagent>> tests;

};

#endif /* _ATCHANDLER_H */
