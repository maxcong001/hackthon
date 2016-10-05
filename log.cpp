#include <log4cplus/logger.h>
//#include <log4cplus/consoleappender.h>
#include <log4cplus/fileappender.h>
#include <log4cplus/layout.h>
//#include <log4cplus/ndc.h>
//#include <log4cplus/mdc.h>
#include <log4cplus/helpers/loglog.h>
#include <log4cplus/thread/threads.h>
//#include <log4cplus/helpers/sleep.h>
#include <log4cplus/loggingmacros.h>
#include <iostream>
#include <string>


using namespace std;
using namespace log4cplus;
using namespace log4cplus::helpers;

int
main()
{
    log4cplus::initialize ();
    try {
        SharedObjectPtr<Appender> append_1(new FileAppender("Test.log"));
        append_1->setName(LOG4CPLUS_TEXT("First"));

        log4cplus::tstring pattern = LOG4CPLUS_TEXT("[%d{%m/%d/%y %H:%M:%S,%Q}] %c %-5p - %m [%l]%n");
        //  std::tstring pattern = LOG4CPLUS_TEXT("%d{%c} [%t] %-5p [%.15c{3}] %%%x%% - %m [%l]%n");
        append_1->setLayout( std::auto_ptr<Layout>(new PatternLayout(pattern)) );
        Logger::getRoot().addAppender(append_1);

        Logger logger = Logger::getInstance(LOG4CPLUS_TEXT("Max:"));
        logger.setLogLevel(TRACE_LOG_LEVEL);
        LOG4CPLUS_DEBUG(logger, "This is the FIRST log message..."<<"test");

    }
    catch(...) {
        Logger::getRoot().log(FATAL_LOG_LEVEL, LOG4CPLUS_TEXT("Exception occured..."));
    }

    return 0;
}

