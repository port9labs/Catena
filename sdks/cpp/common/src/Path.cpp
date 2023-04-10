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

#include <Path.h>
#include <utils.h>

#include <regex>

catena::Path::Path(const std::string& path) : segments_{} {
    // regex will split a well-formed json pointer into a sequence of strings
    // that all start with the solidus '/' character and contain these matches:
    // /- -- can be used as an array index
    // /any_string_that_uses_only_word_chars -- i.e. letters & underscores.
    // /string_with_~0_~1_escaped_chars -- string_with_~_/_escaped_chars
    // /291834719 -- just numbers
    //
    std::regex segment_regex("(\\/-{1})|(\\/([\\w]|~[01])*)");
    auto r_begin = std::sregex_iterator(path.begin(), path.end(), segment_regex);
    auto r_end = std::sregex_iterator();
    for (std::sregex_iterator it = r_begin; it != r_end; ++it) {
    std::smatch match = *it;
    if (it == r_begin && match.position(0) != 0) {
        std::stringstream why;
        why << __PRETTY_FUNCTION__ << "\n'" << path << "' must begin with '/'";
        throw std::runtime_error(why.str());
    }

    std::string txt = unescape(match.str());
    // strip off the leading solidus '/'
    segments_.push_back(txt.substr(1, std::string::npos));
    }
    std::cout << std::endl;
}

catena::Path::Path(const char* literal) : Path(std::string(literal)) {}

std::string catena::Path::pop_front() {
  if (segments_.size() >= 1) {
    std::string ans = segments_[0];
    segments_.pop_front();
    return ans;
  } else {
    std::stringstream why;
    why << __PRETTY_FUNCTION__ << "\nAttempt to pop empty store.";
    throw std::range_error(why.str());
  }
}

void catena::Path::escape (std::string& str) {
  subs(str, "~", "~0");
  subs(str, "/", "~1");
}

std::string catena::Path::unescape(const std::string& str) {
  std::string ans(str);
  subs(ans, "~0", "~");
  subs(ans, "~1", "/");
  return ans;
}

std::unique_ptr<catena::Path> operator"" _path(const char* lit, std::size_t sz) {
    return std::make_unique<catena::Path>(lit);
}