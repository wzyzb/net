#include "drivermanager.h"

std::map< std::string, IDriverPtr > CDriverManager::m_mapDrivers;
std::map< std::string, IDriverPtr > CDriverManager::m_mapClasses;

CDriverManager::CDriverManager()
{
}

CDriverManager::~CDriverManager()
{
}

bool CDriverManager::registry(IDriver* Driver, bool bReplace)
{
    std::map< std::string, IDriverPtr >::iterator iter_find;
    iter_find = m_mapDrivers.find( Driver->getDriverName() );

    if ( iter_find != m_mapDrivers.end() )
    {
        if( bReplace )
        {
            iter_find->second = Driver;
            return true;
        }
        else
        {
            return false;
        }
    }
    std::pair<std::map< std::string, IDriverPtr >::iterator, bool> iter_ret;
    iter_ret = m_mapDrivers.insert( std::map< std::string, IDriverPtr >::value_type(Driver->getDriverName(), Driver) );		
    return iter_ret.second;
}

IDriver* CDriverManager::getDriver(const char* DriverName)
{
    std::map< std::string, IDriverPtr >::iterator iter_find;
    iter_find = m_mapDrivers.find( DriverName );

    if ( iter_find != m_mapDrivers.end() )
    {
        return iter_find->second.get();
    }
    return NULL;
}

bool CDriverManager::registryByClassName(IDriver* Driver, bool bReplace)
{
    std::map< std::string, IDriverPtr >::iterator iter_find;
    iter_find = m_mapClasses.find( Driver->getClassName() );

    if ( iter_find != m_mapClasses.end() )
    {
        if( bReplace )
        {
            iter_find->second = Driver;
            return true;
        }
        else
        {
            return false;
        }
    }
    std::pair<std::map< std::string, IDriverPtr >::iterator, bool> iter_ret;
    iter_ret = m_mapClasses.insert( std::map< std::string, IDriverPtr >::value_type(Driver->getClassName(), Driver) );		
    return iter_ret.second;
}

IDriver* CDriverManager::getDriverByClassName(const char* ClassName)
{
    std::map< std::string, IDriverPtr >::iterator iter_find;
    iter_find = m_mapClasses.find( ClassName );

    if ( iter_find != m_mapClasses.end() )
    {
        return iter_find->second.get();
    }
    return NULL;
}

