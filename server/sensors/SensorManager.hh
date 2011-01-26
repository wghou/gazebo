/*
 * Copyright 2011 Nate Koenig & Andrew Howard
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
/*
 * Desc: Class to manager all sensors
 * Author: Nate Koenig
 * Date: 18 Dec 2009
 * SVN info: $Id$
 */

#ifndef SENSORMANAGER_HH
#define SENSORMANAGER_HH

#include <list>

#include "SingletonT.hh"

namespace gazebo
{
  class Sensor;

  /// \brief Class to manage and update all sensors
  class SensorManager : public SingletonT<SensorManager>
  {
    /// \brief Constructor
    public: SensorManager();

    /// \brief Destructor
    public: virtual ~SensorManager();

    /// \brief Update all the sensors
    public: void Update();

    /// \brief Init all the sensors
    public: void Init();

    /// \brief Finalize all the sensors
    public: void Fini();

    /// \brief Add a sensor
    public: void AddSensor(Sensor *sensor);

    /// \brief Remove a sensor
    public: void RemoveSensor(Sensor *sensor);

    private: std::list<Sensor *> sensors;

    private: friend class DestroyerT<SensorManager>;
    private: friend class SingletonT<SensorManager>;
  };
}

#endif
