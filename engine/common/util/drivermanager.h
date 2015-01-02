#ifndef _DRIVERMANAGER_H_
#define _DRIVERMANAGER_H_

#include <map>
#include <string>
#include "driver.h"

class CDriverManager
{
public:
	CDriverManager();
	virtual ~CDriverManager();

	/**
	 *
	 */
	static bool registry(IDriver* Driver, bool bReplace = true);

	/**
	 *
	 */
    static IDriver* getDriver(const char* DriverName);

	/**
	 *
	 */
	static bool registryByClassName(IDriver* Driver, bool bReplace = true);

	/**
	 *
	 */
    static IDriver* getDriverByClassName(const char* DriverName);

private:
	static std::map< std::string, IDriverPtr > m_mapDrivers; 
	static std::map< std::string, IDriverPtr > m_mapClasses;
};	

#endif
