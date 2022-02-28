/**
 * Copyright 2016 Comcast Cable Communications Management, LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <rdk_debug.h>
#include <time.h>

#include "cimplog.h"

#define LOG_FILE "/tmp/cimplog.txt"
#define DBUG_PRINT(fmt ...)     {\
                                        FILE     *fp        = NULL;\
                                        fp = fopen ( LOG_FILE, "a+");\
                                        if (fp)\
                                        {\
                                            fprintf(fp,fmt);\
                                            fclose(fp);\
                                        }\
                               }\


#define DEBUG_INI_NAME		"/etc/debug.ini"
#define MAX_BUF_SIZE 1024

const char *__attribute__((weak)) rdk_logger_module_fetch(void);
static int init_done = 0;

char buffer[26];
char * timeGet() {
    time_t timer;
    struct tm* tm_info;

    timer = time(NULL);
    tm_info = localtime(&timer);

    strftime(buffer, 26, "%Y-%m-%d %H:%M:%S", tm_info);
    return buffer;
}

void __cimplog(const char *module, int level, const char *msg, ...)
{
    static const char *rdk_logger_module = NULL;
    if((module != NULL) && (strcmp(module, "Aker") == 0)) {
    DBUG_PRINT("CIMPLOG : module %s init_done in __cimplog = %d & time is : %s\n", module, init_done, timeGet());
    }
    if( !init_done )
    {
        rdk_logger_module = rdk_logger_module_fetch();
        if( NULL == rdk_logger_module )
        {
            //If RDK logger is not integrated, give the information on the 'stderr' to use the weak implementation of
            //'const char *rdk_logger_module_fetch(void)' method, through which the rdk_logger_module can be obtained.
            fprintf(stderr, "\nERROR: RDK Logger not integrated for this module - '%s' !!!\n",module);
            fprintf(stderr, " Provide cimplog method \"const char *rdk_logger_module_fetch(void)\" to get log prints !!\n");
	    DBUG_PRINT("CIMPLOG :  RDK Logger not integrated for this module - %s !!! and time is %s\n", module, timeGet());
        }
        else
        {
            DBUG_PRINT("CIMPLOG : __cimplog time in module %s before RDK_LOGGER_INIT Time : %s\n", module, timeGet());		
            RDK_LOGGER_INIT();
	    DBUG_PRINT("CIMPLOG : __cimplog time in module %s after RDK_LOGGER_INIT Time : %s\n", module, timeGet());
        }
        init_done = 1;
    }

    if( NULL == rdk_logger_module )
    {
        //If RDK logger module is not defined, use __cimplog_generic() to capture the logs.
        //The logs will be capture to /rdklogs/logs/Consolelog.txt.0 if available or it
        //will be printed on the console(stdout)
	DBUG_PRINT("CIMPLOG : module %s rdk_logger_module not defined, use __cimplog_generic() to capture the logs time is %s\n", module, timeGet());
        __cimplog_generic(module, msg);
        return;
    }

    //else print to RDK Logger
    static const rdk_LogLevel _level[] = { RDK_LOG_ERROR, RDK_LOG_INFO, RDK_LOG_DEBUG };
    va_list arg_ptr;
    char buf[MAX_BUF_SIZE];
    int nbytes;

    if (level <= LEVEL_INFO)
    {   
	if((module != NULL) && (strcmp(module, "Aker") == 0)) {    
	DBUG_PRINT("CIMPLOG : module %s print to RDK Logger in func __cimplog time is %s\n", module, timeGet());
	}
        va_start(arg_ptr, msg);
        nbytes = vsnprintf(buf, MAX_BUF_SIZE, msg, arg_ptr);
        va_end(arg_ptr);

        if( nbytes >=  MAX_BUF_SIZE )	
        {
            buf[ MAX_BUF_SIZE - 1 ] = '\0';
        }
        else
        {
            buf[nbytes] = '\0';
        }

        RDK_LOG(_level[0x3 & level], rdk_logger_module, "%s: %s", module, buf);
    }

    return;
}

// The below 'weak' linkage should be replaced with strong definition in the module integrating to cimplog
const char *rdk_logger_module_fetch(void)
{
    return NULL;
}


// To be used for additional rdk logging to different file other the primary logging
void __cimplog_rdk_generic(const char *rdk_logger_module, const char *module, int level, const char *msg, ...)
{
    if((module != NULL) && (strcmp(module, "Aker") == 0)) {	
    DBUG_PRINT("CIMPLOG : module %s init_done in __cimplog_rdk_generic = %d & time is : %s\n", module, init_done, timeGet());
    }
    if( !init_done )
    {
	DBUG_PRINT("CIMPLOG : __cimplog_rdk_generic time in module %s before RDK_LOGGER_INIT Time : %s\n", module, timeGet());    
        RDK_LOGGER_INIT();
	DBUG_PRINT("CIMPLOG : __cimplog_rdk_generic time in module %s after RDK_LOGGER_INIT Time : %s\n", module, timeGet());
        init_done = 1;
    }

    if( NULL == rdk_logger_module )
    {
        //if RDK logger module is not defined, simple return - dont print.
        // used when calling module is not interested in log prints
	DBUG_PRINT("CIMPLOG : module %s rdk_logger_module is not defined in __cimplog_rdk_generic time is : %s\n", module, timeGet());
        return;
    }

    //else print to RDK Logger
    static const rdk_LogLevel _level[] = { RDK_LOG_ERROR, RDK_LOG_INFO, RDK_LOG_DEBUG };
    va_list arg_ptr;
    char buf[MAX_BUF_SIZE];
    int nbytes;

    if (level <= LEVEL_INFO)
    {
	if((module != NULL) && (strcmp(module, "Aker") == 0)) {    
	DBUG_PRINT("CIMPLOG : module %s print to RDK Logger in func cimplog_rdk_generic time is %s\n", module, timeGet()); 
	}
        va_start(arg_ptr, msg);
        nbytes = vsnprintf(buf, MAX_BUF_SIZE, msg, arg_ptr);
        va_end(arg_ptr);

        if( nbytes >=  MAX_BUF_SIZE )	
        {
            buf[ MAX_BUF_SIZE - 1 ] = '\0';
        }
        else
        {
            buf[nbytes] = '\0';
        }
        RDK_LOG(_level[0x3 & level], rdk_logger_module, "%s: %s", module, buf);
    }

    return;
}

