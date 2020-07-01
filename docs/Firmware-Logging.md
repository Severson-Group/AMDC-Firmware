# Logging with the AMDC
The ability to log and extract variables of interest out of the AMDC is a critical feature needed for debugging, testing, and general data recording. Because of this, the AMDC has logging capabilities built into the firmware and can record up to 32 variables. The intent of this document is to show a user how to implement logging in their code. 

There are two interfaces that can be used for logging: 1) the standard serial terminal interface that is typically used with user commands (logging can be thought of as an application that can be included in your project) and 2) a Python interface that is built on top of and wraps the serial interface. It is highly recommended that users use the Python interface as it provides certain convenience methods and abstractions that make logging much more intuitive and less error prone.

## General Flow

The general flow for logging data in the AMDC is simple and uses the procedure shown below. Note that steps 2-5 are done through the python interface:

1. Set up user C-code for logging
1. Register variable(s) to log
1. Start logging
1. Stop logging
1. Dump the collected data

## Internal Workings

Internally...

## C-Code Modifications

The firmware has been design specifically to limit the amount of changes users have to make to their C-code to log variables of interest. The only modifications that users need to make to their C-code are as follows:

1. For every variable that you want to log within your code, create a new global variable with the same name prepended by `LOG_` (note that it is case sensitive). For example, if you have a variable `foo` in your code that you would like to log, create a new global variable of the same type called `LOG_foo`.

1. Update all global logging variables wherever desired by assigning the local variable to the global variable (e.g. `LOG_foo = foo;`)

#### Example
The following example illustrates one possible use case:

We have a typedef called `Currents_t` that is a struct containing measured currents from each of the three inverter phases. This variable is then updated by the generic `read_currents` function. You could imagine this function reading in the three current sensors from an inverter. We wish to log the three phase currents. To do this we use the two steps listed above. First we create global variables for each of the three currents that we care about. Then in the callback function we update the global current variables to equal the measured currents that we care about tracking. Note that in this example we update the global variables within the callback but you can update them at any point in your code. For example, we could have updated the global variables inside of the `read_currents()` function

```
double LOG_Ia = 0.0;
double LOG_Ib = 0.0;
double LOG_Ic = 0.0;

typedef struct Currents_t {

    double Ia;
    double Ib;
    double Ic;

} Currents_t;

static Currents_t Iabc = {0.0, 0.0, 0.0};

void callback_func(){

    //read currents from sensors
    read_currents(&Iabc);
    
    //updating variables that are being recorded by logging application
    LOG_Ia = Iabc.Ia;
    LOG_Ib = Iabc.Ib;
    LOG_Ic = Iabc.Ic;
}
```

## Terminal Interface

The terminal interface operates the same as any other user application (as shown below). It is highly recommended, however, that the user does not use the terminal interface directly, as it is difficult to use and requires a lot of book keeping. The following documentation will describe each of terminal commands, but is primarily just for completeness of the documentation.

<img src="images/logging/terminal.png" />

The logging application has the following commands:

1. `reg` -- registers a new variable for logging
    > **Required Arguments**  
    > - `log_var_idx` -- the index that you want the variable to be stored in (must be 0-31). The command will fail if a variable is already registered in the requested slot.
    > - `name` -- name of the variable that you are logging (example: `LOG_foo`)
    > - `memory_addr` -- global memory address of the variable you are logging in decimal format. The reason global variables are created for logging is because their address remains constant at runtime. The memory address can be found in "mapfile.txt" in a hexadecimal format, which is located in the "Debug" folder of the users private c code. After locating the variable's address, you must convert it from hexadecimal to decimal before entering it in the terminal.
    > - `samples_per_sec`  -- the sample rate in samples per second that you wish to record the variable at. Note that not all variables have to have the same sample rate.
    > - `type` -- data type of the variable being logged. Valid types are: `double`, `float`, `int` 

2. `unreg` -- unregisters a variable that you no longer care to log  
    > **Required Arguments**  
    > - log_var_idx -- the index of the variable that you want to unregister (must be 0-31).
    
3. `start` -- starts recording data  
    
4. `stop` -- stops recording data  
    
5. `dump` -- dumps all of the recorded data of a slot out to the serial terminal  
    > **Required Arguments**  
    > - `bin` or `text` -- One of the preceding flags must be set. If `bin` is used, the data will be dumped to the serial terminal in binary format. If `text` is used, the data will be dumped to the serial terminal in human readable text format. Using `bin` is much faster.
    > - `log_var_idx` -- index of the variable that you wish to dump (must be 0-31)
    
6. `empty` -- resets the index of a logging slot (calling `dump` after `empty` on the same slot will result in no data being output)  
    > **Required Arguments**  
    > - `log_var_idx` -- index of the variable you wish to reset
    
7. `info` -- prints information about what variables are being logged and what slots are taken to the serial terminal  

## Python Interface

Before you can use the python interface, you must modify your c code according to the C-Code Modifications [section](#c-code-modifications).

Note that in the text that follows, `REPO_DIR` is an alias for the file path to where your repository is located. `REPO_DIR` contains the `AMDC-Firmware` submodule as well as a the folder containing your user c code.

The python interface is built on top of the serial terminal logging interface in the sense that it simply enters commands at the serial terminal for you. What makes this so advantageous, however, is that python can take care of all of the book keeping for you by remembering which variables are being logged, which variables are in which slots, and which slots are still available. It also automatically determines the memory addresses of each variable and converts them to correct format, reads dumped data off of the serial terminal and converts it to a format that can be saved to a `.csv` file and has other convenience functions that make logging much easier. 

#### 1. Import needed modules

To use logging in python, you must `import` the `AMDC` and `AMDC_Logger` modules from the scipts folder of the AMDC-Firmware. There are two main classes that you need to be concerned with:

1. `AMDC`: class that is found in the `AMDC` module. Responsible for communicating with the AMDC over serial terminal
2. `AMDC_Logger`: class that is found in the `AMDC_Logger` module. Responsible for sending logging commands to the AMDC and book keeping

The top of your python script should look like the following:

```
import sys
scripts_folder = r'REPO_DIR\AMDC-Firmware\scripts'
sys.path.append(scripts_folder)

from AMDC import AMDC
from AMDC_Logger import AMDC_Logger, find_mapfile
```

Adding the location of the scripts folder to the `sys.path` variable allows python to find the `AMDC` and `AMDC_Logger` modules to import them.

After importing the modules, perform the following steps:

#### 2. Instantiate an `AMDC` object and connect it to the AMDC:
```
amdc = AMDC(port = 'COM4')  
amdc.connect() #opens up serial communication
```

Note that you may need to change the port number (i.e. `COM4' --> 'COM3', etc.) depending on which port the AMDC is communicating to your computer through. You can determine this by first connecting to the AMDC through the terminal and noting which port it tries to automatically connect with. 

#### 3. Instantiate an `AMDC_Logger` object:
```
mapfile_path = find_mapfile(REPO_DIR)
logger = AMDC_Logger(AMDC = amdc, mapfile = mapfile_path)
```

The `AMDC_Logger` object requires two inputs on instantiation: an `AMDC` object (created in step 2), and a file path to where the mapfile is located. You can manually locate and specifiy the location of `mapfile.txt` or you can use the convenience function `find_mapfile()` which takes in the base path of the repository and locates and returns the path to the mapfile.

#### 4. Synchronize logger with AMDC:

```
logger.sync()
```

This step isn't required but is recommended. It reads the current state of logging in the AMDC and synchronizes python to that state. It's useful for if you restart your python session while the amdc is still on. If you don't do this and variables are are set up for logging in the AMDC, the internal state of python's book keeping and the AMDC won't align and you'll get unexpected behavior.

#### 5. Register variables of interest:  
There are several ways to register variables for logging. One way is as follows:
```
logger.register('LOG_foo', samples_per_sec = 1000, var_type = 'double')
```
Note that register has default arguments of `samples_per_sec = 1000` and `var_type = 'double'` so the preceding line could also be accomplished as follows:
```
logger.register('LOG_foo')
```

If you have multiple variables that you wish to register with the same type and sample rate you can register them all at the same time. the `AMDC_Logger` class is also smart and sanitizes the input variables so you don't have to prepend `LOG_` to each variable if you don't want. The following snippets of code all accomplish the same task.

```
logger.register('LOG_foo LOG_bar LOG_baz') # variable names in one string seperated by white space
logger.register('foo bar baz')             # one string with no LOG_ (this option is probably the fastest/easiest)
logger.register(['foo', 'bar', 'baz'])     #list of variable names no
logger.register(('foo', 'bar', 'baz'))     #tuple of variable names
```

There is also a convenient `auto_register()` function that can be used to search your user code for variables of the form `LOG_*` and register them for you automatically. You just give the file path to your app's c code as follows:

```
logger.auto_register(path_to_user_app)
```

if you want to check to see which variables the auto register function will register before calling it, you can call the `auto_find_vars()` function as follows:
```
log_vars, log_types = auto_find_vars(path_to_user_app)
```
where `log_vars` is a list containing all of the variables found in the user c code and `log_types` is a list containing the corresponding variable types.

#### 6. Clear logged variables:
```
logger.clear_all()
```
Calling the `clear_all()` method resets all of the internal indices of the logger so that you don't receive old logged data. You can kind of think of it as clearing the log. If you just wish to clear a single variable for some reason you can call the `clear()` method and pass in the name of the variable you wish to clear. NOTE: clearing variables does not unregister them from logging.

```
logger.clear('foo') #clear single variable
```


#### 7. Start logging:
```
logger.start()
```

#### 8. Stop logging:
```
logger.stop()
```

Typically, you will want to record an event or to record data for a set amount of time. Because of this, it is common to import the `time` module and to use the `sleep()` function which expects a delay time in seconds. The following example illustrates a common use case:

```
logger.clear_all()
logger.start()

do_something()
time.sleep(3) #record data for 3 seconds

logger.stop()
```

#### 9. Dump data:

After collecting data, you will want to access that data. You do that as follows:  
```
data = logger.dump()
```
The output of the `dump()` method is a `pandas` `DataFrame`. `pandas` is a super popular data science library in python and a `DataFrame` is the primary object that pandas works with. The columns of the `DataFrame` correspond to each logged variable and the index of the dataframe is time.

The `dump()` function is really powerful and has a lot of optional arguments. By default dump will dump out all logged variables. This can be time consuming though, so if you want, you can specifiy a subset of variables to dump as follows:

```
data = logger.dump(log_vars = 'foo bar')
```

You can also specify a file path and `dump()` will automatically save your data to a `.csv` file. This is nice to make sure your data is persistent between experiments. By default, the `dump()` function appends a timestamp to your file name so that you can't accidently overwrite data that you've collected. 

```
data = logger.dump(log_vars = 'foo bar', file = 'my_data.csv')
```

Sometimes it is nice to add notes to a specific set of data. You can do this by adding the additional optional parameter `comment` to the `dump()` function.

```
data = logger.dump(log_vars = 'foo bar', file = 'my_data.csv', comment = 'the motor appeared to run smooth')
```

#### Example

Now that your data is in a `DataFrame` you can post-process it however you wish. As a bit of motivation for of why `DataFrames` are powerful for logging and debugging, consider the following example. 

Imagine we have recorded position data from `x`, `y`, and `z` displacement sensors as well as measured three phase currents `Ia`, `Ib`, and `Ic`. We can extract all of the data into a single dataframe and save the data as follows:

```
data = logger.dump(file = 'sensed_values.csv')
```

Now we can make a plot of only the position data in a single line by calling the `plot()` method on the `DataFrame` while indexing into the displacement data:

```
data['x y z'.split()].plot()
```
Note that the above single line is equivalent to the following:

```
pos_vars = ['x', 'y', 'z']
pos_data = data[pos_vars] #index into dataframe to get position data columns (returns new dataframe)
pos_data.plot()
```

This is just one example of how quick `pandas` can make visualizing our logged data for debugging and quick inspection.

### Miscellaneous Log Functions

1. Log for set duration
```
logger.log(duration = 0.5) #record data for half second
```
The above is exactly equivalent to
```
logger.start()
time.sleep(0.5)
logger.stop()
```

1. Unregister Variables
Perhaps for some reason you want to unregister some variables, that can be done easily using the `unregister()` method as follows:

```
logger.unregister('foo bar')
```
Again the input can take many forms, the following would also work:
```
logger.unregister('LOG_foo LOG_bar')
logger.unregister(['LOG_foo', LOG_bar'])
logger.unregister(['foo', 'bar'])
...
```
If for some reason you want to unregister all variables, you can do that as the following:

```
logger.unregister_all()
```

1. Load saved data
At some point you'll probably want to load in an old run of data. You can do this using the `load()` method as follows:
```
data = logger.load('old_data_file.csv')
```
This will load your old data run into a `pandas` `DataFrame`. The load function is just a thin wrapper around the `pandas` `read_csv()` method and the above line of code is equivalent to:
```
data = pd.read_csv('old_data_file.csv', comment = '#', index_col = 't')
```
loading the data this way sets time to be the index of the `DataFrame` and ignores any comments you may have stored with the data.

