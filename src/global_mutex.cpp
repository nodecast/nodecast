#include "global_mutex.h"


namespace global_mutex {
    QMutex thumbnail_mutex(QMutex::Recursive);
}
