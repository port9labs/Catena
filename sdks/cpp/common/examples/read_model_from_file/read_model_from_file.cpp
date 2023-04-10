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


/**
 * @brief Reads a catena device model from a JSON file and writes it to stdout.
 * 
 * Design intent: provide a handy way to validate (potentially) human-
 * authored device models. If the model is empty, the input is faulty.
 * 
 * Note that items in the input model that have default values (0 for ints, 
 * false for booleans, ...) will be stripped from the model that's output.
 * 
 * @author John R. Naylor (john.naylor@rossvideo.com)
 * @file read_model_from_file.cpp
 * @copyright Copyright © 2023, Ross Video Ltd
 */

 #include <DeviceModel.h>
 #include <Path.h>

 #include <iostream>
 #include <stdexcept>

 int main(int argc, char** argv) {
    // process command line
    if (argc != 2) {
        std::cout << "usage: " << argv[0] << " path/to/input-file.json\n";
        exit(EXIT_SUCCESS);
    }

    try {
        // read a json file into a DeviceModel object
        // we don't need this one to be threadsafe, so use false
        // as the template parameter
        catena::DeviceModel<false> dm(argv[1]);

        // write the device model to stdout
        std::cout << "Read Device Model: " << dm << '\n';

        // get a value from the device model
        float v{};
        catena::Param& param = dm.getValue(v, "/hello");
        std::cout << "param oid: '" << dm.getOid(param) 
            << "' has value: " << v << '\n';

        // set a value in the device model
        std::cout << "setting it to something different\n";
        dm.setValue("/hello", 3.142f);
        //dm.setValue(param, 3.142f); is preferable, more efficient


        // write out the updated device model
        std::cout << "Updated Device Model: " << dm << '\n';

        std::string serialized;
        dm.device().SerializeToString(&serialized);
        std::cout << "Device model serializes to " << serialized.size() << " bytes\n";

    } catch (std::exception& why) {
        std::cerr << "Problem: " << why.what() << '\n';
        exit(EXIT_FAILURE);
    }
    return EXIT_SUCCESS;
 }