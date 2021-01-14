/*
      This file is part of Smoothie (http://smoothieware.org/). The motion control part is heavily based on Grbl (https://github.com/simen/grbl).
      Smoothie is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
      Smoothie is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
      You should have received a copy of the GNU General Public License along with Smoothie. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef KERNEL_H
#define KERNEL_H

#define THEKERNEL Kernel::instance
#define THECONVEYOR THEKERNEL->conveyor
#define THEROBOT THEKERNEL->robot

#include "Module.h"
#include "I2C.h" // mbed.h lib
#include <array>
#include <vector>
#include <string>

//Module manager
class Config;
class Module;
class Conveyor;
class SlowTicker;
class SerialConsole;
class StreamOutput;
class StreamOutputPool;
class GcodeDispatch;
class Robot;
class Planner;
class StepTicker;
class Adc;
class PublicData;
class SimpleShell;
class Configurator;

enum STATE {
	IDLE   = 0,
	RUN    = 1,
	HOLD   = 2,
	HOME   = 3,
	ALARM  = 4,
	SLEEP  = 5
};

enum HALT_REASON {
	// No need to reset when triggered
	MANUAL     				= 1,
	HOME_FAIL  				= 2,
	PROBE_FAIL 				= 3,
	CALIBRATE_FAIL			= 4,
	ATC_HOME_FAIL   		= 5,
	ATC_NO_TOOL				= 6,
	ATC_HAS_TOOL			= 7,
	SPINDLE_OVERHEATED 		= 8,
	SOFT_LIMIT				= 9,
	// Need to reset when triggered
	HARD_LIMIT				= 11,
	MOTOR_ERROR_X			= 12,
	MOTOR_ERROR_Y			= 13,
	MOTOR_ERROR_Z			= 14,
	SPINDLE_ERROR			= 15
};

typedef struct {
	float TLO;
	int TOOL;
	float G54[3];
	float G28[3];
} EEPROM_data;

class Kernel {
    public:
        Kernel();

        ~Kernel() {
            delete this->i2c;
            delete this->eeprom_data;
        }

        static Kernel* instance; // the Singleton instance of Kernel usable anywhere
        const char* config_override_filename(){ return "/sd/config-override"; }

        void add_module(Module* module);
        void register_for_event(_EVENT_ENUM id_event, Module *module);
        void call_event(_EVENT_ENUM id_event, void * argument= nullptr);

        bool kernel_has_event(_EVENT_ENUM id_event, Module *module);
        void unregister_for_event(_EVENT_ENUM id_event, Module *module);

        bool is_using_leds() const { return use_leds; }
        bool is_halted() const { return halted; }
        bool is_grbl_mode() const { return grbl_mode; }
        bool is_ok_per_line() const { return ok_per_line; }

        void set_feed_hold(bool f) { feed_hold= f; }
        bool get_feed_hold() const { return feed_hold; }
        bool is_feed_hold_enabled() const { return enable_feed_hold; }
        void set_bad_mcu(bool b) { bad_mcu= b; }
        bool is_bad_mcu() const { return bad_mcu; }

        void set_uploading(bool f) { uploading = f; }
        bool is_uploading() const { return uploading; }

        void set_laser_mode(bool f) { laser_mode = f; }
        bool get_laser_mode() const { return laser_mode; }

        void set_sleeping(bool f) { sleeping = f; }
        bool is_sleeping() const { return sleeping; }

        void set_halt_reason(uint8_t reason) { halt_reason = reason; }
        uint8_t get_halt_reason() const { return halt_reason; }

        void read_eeprom_data();
        void write_eeprom_data();
        void erase_eeprom_data();

        std::string get_query_string();
        void query_hmi(StreamOutput *stream);

        // These modules are available to all other modules
        SerialConsole*    serial;
        StreamOutputPool* streams;
        GcodeDispatch*    gcode_dispatch;
        Robot*            robot;
        Planner*          planner;
        Config*           config;
        Conveyor*         conveyor;
        Configurator*     configurator;
        SimpleShell*      simpleshell;

        SlowTicker*       slow_ticker;
        StepTicker*       step_ticker;
        Adc*              adc;
        std::string       current_path;
        uint32_t          base_stepping_frequency;

        uint8_t get_state();
        uint8_t halt_reason;
        EEPROM_data *eeprom_data;

    private:
        // When a module asks to be called for a specific event ( a hook ), this is where that request is remembered
        mbed::I2C* i2c;
        std::array<std::vector<Module*>, NUMBER_OF_DEFINED_EVENTS> hooks;
        struct {
            bool use_leds:1;
            bool halted:1;
            bool grbl_mode:1;
            bool feed_hold:1;
            bool ok_per_line:1;
            volatile bool enable_feed_hold:1;
            bool bad_mcu:1;
            volatile bool uploading:1;
            bool laser_mode:1;
            bool sleeping:1;
        };

};

#endif
