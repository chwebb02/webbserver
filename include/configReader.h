#ifndef _CONFIG_READER_H_
#define _CONFIG_READER_H_

#include "config.h"

/// @brief Reads a configuration file to configure the server
/// @param propertiesFile The filepath of the configuration file
/// @return A config object, or NULL on error
config_t *readConfigurationFile(const char *propertiesFile);

/// @brief Release memory associated with config
/// @param conf The config object
void deleteConfig(config_t *conf);

#endif /* _CONFIG_READER_H_ */