#pragma once
/*
 * misc.hpp
 *
 *  Created on: Sep 12, 2014
 *      Author: nickhathaway
 */


#include <string>
#include <sstream>


namespace estd {
/**@b simply aesthetic, to make call to is_arithmetic look nicer
 *
 * @return Returns true if T is of type arithmetic
 */
template<typename T>
constexpr bool isArithmetic(){
	return std::is_arithmetic<T>::value;
}


/**@b for determining if T is of type char* or const char* to declare that is a string
 *
 */
template<typename T>
struct is_string : public std::integral_constant<bool, std::is_same<char*, typename std::decay<T>::type>::value
|| std::is_same<const char*, typename std::decay<T>::type>::value> {};

/**@b Declared so that is_string will return true for std::string along with char* and const char*
 *
 */
template<>
struct is_string<std::string> : std::true_type {};


/**@b Nicer to_string than the std:: because it won't have trailing zeros for doubles
 *
 * @param e The element to change into string, needs to have operator << declared
 * @return Return what ever operator << puts into the stream
 */

/**@b simply aesthetic, to make call to is_string look nicer
 *
 * @return Returns true if T is of type arithmetic
 */
template<typename T>
constexpr bool isString(){
	return is_string<T>::value;
}


/**@b Templated function to pass anything to string as long as it has an << operator
 *
 * @param e Element to change to a string
 * @return The element chaged into a string
 */
template <typename T>
std::string to_string(T e) {
  std::stringstream ss;
  ss << e;
  return ss.str();
}

/**@b change for bool objects to be translated to true or false, could cover up if true is a different number
 *
 * @param obj The bool to translate to a string
 * @return True if obj is true and false if obj is false
 */
template <>
inline std::string to_string<bool>(bool obj) {
	if(obj){
		return "true";
	}else{
		return "false";
	}
}

//  added to std:: stou, stous, stos because they are not in the standard
// library
//  so that they have similar behavior to the other string conversions
inline uint32_t stou(const std::string& str, size_t* idx = 0, int base = 10) {
  auto firstConverion = std::stol(str, idx, base);
  if (firstConverion < 0 ||
      firstConverion > std::numeric_limits<uint32_t>::max()) {
    throw std::out_of_range{"stou"};
  }
  return static_cast<uint32_t>(firstConverion);
}
inline uint16_t stous(const std::string& str, size_t* idx = 0, int base = 10) {
  auto firstConverion = std::stol(str, idx, base);
  if (firstConverion < 0 ||
      firstConverion > std::numeric_limits<uint16_t>::max()) {
    throw std::out_of_range{"stous"};
  }
  return static_cast<uint16_t>(firstConverion);
}
inline int16_t stos(const std::string& str, size_t* idx = 0, int base = 10) {
  auto firstConverion = std::stoi(str, idx, base);
  if (firstConverion < std::numeric_limits<int16_t>::min() ||
      firstConverion > std::numeric_limits<int16_t>::max()) {
    throw std::out_of_range{"stos"};
  }
  return static_cast<int16_t>(firstConverion);
}

}  // namespace estd
