// Licensed under the Creative Commons Attribution NoDerivatives 4.0 
// International Licensing (CC-BY-ND-4.0);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at:
//
// https://creativecommons.org/licenses/by-nd/4.0/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include <DeviceModel.h>
#include <utils.h>
#include <Path.h>

#include <google/protobuf/map.h>
#include <google/protobuf/util/json_util.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>





template<bool THREADSAFE>
catena::DeviceModel<THREADSAFE>::DeviceModel(const std::string& filename)
    : device_{} {
  auto jpopts = google::protobuf::util::JsonParseOptions{};
  std::string file = catena::readFile(filename);
  google::protobuf::util::JsonStringToMessage(file, &device_, jpopts);
}

template<bool THREADSAFE>
const catena::Device& catena::DeviceModel<THREADSAFE>::device() const {
  LockGuard_t lock(mutex_);
  return device_;
}

template<bool THREADSAFE>
catena::Param& catena::DeviceModel<THREADSAFE>::getParam(const std::string& path) {
  // simple implementation for now, only handles flat params
  LockGuard_t lock(mutex_);
  catena::Path path_(path);
  if (path_.size() != 1) {
    std::stringstream why;
    why << __PRETTY_FUNCTION__ << " implementation limit, can only handle"
      "flat parameter structures at this time.";
    throw std::runtime_error(why.str());
  }

  // get our oid and look for it in the array of params
  std::string oid = path_.pop_front();
  int nParams = device_.mutable_params()->descriptors_size();
  PDesc_t* descs = device_.mutable_params()->mutable_descriptors();
  bool found = false;
  int pdx = 0; // param index
  while (pdx < nParams && !found) {
    std::string match = descs->Get(pdx).basic_param_info().oid();
    if (oid.compare(match) == 0) {
      found = true;
    } else {
      ++pdx;
    }
  }
  if (!found) {
    std::stringstream why;
    why << __PRETTY_FUNCTION__;
    why << "Failed to find oid '" << oid << "'\n";
    throw std::runtime_error(why.str());
  }
  return descs->at(pdx);
}

template<bool THREADSAFE>
template<typename T>
catena::Param& catena::DeviceModel<THREADSAFE>::getValue(T& ans, const std::string& path) {
  LockGuard_t lock(mutex_);
  catena::Param& param = getParam(path);
  ans = param.value().float32_value();
  return param;
}

template catena::Param& catena::DeviceModel<true>::getValue<float>(float& ans, const std::string& path);
template catena::Param& catena::DeviceModel<false>::getValue<float>(float& ans, const std::string& path);

template<bool THREADSAFE>
template<typename T>
void catena::DeviceModel<THREADSAFE>::getValue(T& ans, const catena::Param& param) {
  LockGuard_t lock(mutex_);
  
  // N.B. function templates that are members of class templates
  // cannot be specialized, so we have to use conditional compilation based
  // on the tparam instead

  if constexpr(std::is_same<float, T>::value) {
    ans = param.value().float32_value();
  }
}

template<bool THREADSAFE>
template<typename T>
void catena::DeviceModel<THREADSAFE>::setValue(catena::Param& param, T v) {
  LockGuard_t lock(mutex_);

  // N.B. function templates that are members of class templates
  // cannot be specialized, so we have to use conditional compilation based
  // on the tparam instead

  // specialize for float
  if constexpr(std::is_same<T, float>::value) {

    if (param.has_constraint()){
      // apply the constraint
      float min = param.constraint().float_range().min_value();
      float max = param.constraint().float_range().max_value();
      if (v > max) {
        v = max;
      }
      if (v < min) {
        v = min;
      }
    }
    param.mutable_value()->set_float32_value(v);
  }
}

template<bool THREADSAFE>
template<typename T>
catena::Param& catena::DeviceModel<THREADSAFE>::setValue(const std::string& path, T v) {
  catena::Param& param = getParam(path);
  setValue(param, v);
  return param;
}

template catena::Param& catena::DeviceModel<true>::setValue<float>(const std::string& path, float v);
template catena::Param& catena::DeviceModel<false>::setValue<float>(const std::string& path, float v);


template<bool THREADSAFE>
std::ostream& operator<<(std::ostream& os, const catena::DeviceModel<THREADSAFE>& dm) {
   os << printJSON(dm.device());
   return os;
}

/**
 * @brief convenience wrapper around catena::Param
 * 
 * @param param from which to retreive the oid
 * @return param's object id
 */
template<bool THREADSAFE>
const std::string& catena::DeviceModel<THREADSAFE>::getOid(const catena::Param& param) {
  LockGuard_t lock(mutex_);
  return param.basic_param_info().oid();
}

// instantiate the 2 versions of DeviceModel
template class catena::DeviceModel<true>;
template class catena::DeviceModel<false>;


template std::ostream& operator<<(std::ostream& os, const catena::DeviceModel<true>& dm);


template std::ostream& operator<<(std::ostream& os, const catena::DeviceModel<FALSE>& dm);