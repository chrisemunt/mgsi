/*
   ----------------------------------------------------------------------------
   | mgsi                                                                     |
   | Description: Service Integration Gateway                                 |
   | Author:      Chris Munt cmunt@mgateway.com                               |
   |                         chris.e.munt@gmail.com                           |
   | Copyright (c) 2002-2023 MGateway Ltd                                     |
   | Surrey UK.                                                               |
   | All rights reserved.                                                     |
   |                                                                          |
   | http://www.mgateway.com                                                  |
   |                                                                          |
   | Licensed under the Apache License, Version 2.0 (the "License"); you may  |
   | not use this file except in compliance with the License.                 |
   | You may obtain a copy of the License at                                  |
   |                                                                          |
   | http://www.apache.org/licenses/LICENSE-2.0                               |
   |                                                                          |
   | Unless required by applicable law or agreed to in writing, software      |
   | distributed under the License is distributed on an "AS IS" BASIS,        |
   | WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. |
   | See the License for the specific language governing permissions and      |
   | limitations under the License.                                           |
   |                                                                          |
   ----------------------------------------------------------------------------
*/

#ifndef MGSICFG_H
#define MGSICFG_H

int                  mg_site_parameters         ();
int                  mg_get_config              (char *section, char *entry, char *defvalue, char *value, int size, char *file);
int                  mg_set_config              (char *section, char *entry, char *value, char *file);
int                  mg_global_parameters       (void);
int                  mg_system_parameters       (int shndle);
int                  mg_set_global_defaults     (void);
int                  mg_set_system_defaults     (void);
void                 mg_upgrade                 (void);

#endif
