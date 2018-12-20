#pragma once
/*
 * jsonUtils.hpp
 *
 *  Created on: Nov 23, 2014
 *      Author: nickhathaway
 */

#include <json/json.h>
#include <cppitertools/range.hpp>
#include <map>
#include <unordered_map>
#include <vector>
#include <set>
#include <deque>
#include <string>
#include <cstdint>
#include <iostream>
#include <boost/filesystem.hpp>
#include "njhcpp/common.h" //Enable_if
#include "njhcpp/utils/has_member.hpp" //has_member
#include "njhcpp/debug/exception.hpp" //Exception

namespace njh {
/**@brief Namespace for dealing with conversion to json
 *
 */
namespace json {

/**@brief Namepsace to hold all conversion of objects/primitive to json
 *
 */
namespace JsonConversion {

template<typename T>
Json::Value toJson(const T & t);

/**@brief helper func to add the json of a pair to another json so key to value works properly for maps
 *
 * @param pair The pair to convert with the first member is the key and the second is the value
 * @param ret The json object to add to
 * @return A ref to the json object given by ret but with the json added for this pair
 */
template<typename FIRST, typename SECOND>
static Json::Value & toJsonHelper(const std::pair<FIRST, SECOND> & pair,
		Json::Value & ret) {
	ret[estd::to_string(pair.first)] = toJson(pair.second);
	return ret;
}

/**@brief Helper func to add json of this value to another json object
 *
 * @param val The value to convert to json
 * @param ret The json object to add to
 * @return A ref to the json object given by ret but with the json added for this value
 */
template<typename VAL>
static Json::Value & toJsonHelper(const VAL & val, Json::Value & ret) {
	ret.append(toJson(val));
	return ret;
}

struct converter {
	struct check_has_toJson {
		template<typename X, Json::Value (X::*)() const = &X::toJson>
		struct get {
		};
	};

	template<typename X>
	struct class_has_toJson: has_member<X, check_has_toJson> {
	};

	template<typename U>
	static estd::Enable_if<!class_has_toJson<U>::value, Json::Value> toJson(
			const U & con) {
		Json::Value ret;
		for (const auto & val : con) {
			toJsonHelper(val, ret);
		}
		if (ret.isNull()) {
			/**@todo make sure this is only being called for array like objects */
			ret = Json::arrayValue;
		}
		return ret;
	}

	template<typename U>
	static estd::Enable_if<class_has_toJson<U>::value, Json::Value> toJson(
			const U & con) {
		return con.toJson();
	}

	template<typename U>
	static Json::Value toJson(const std::shared_ptr<U> & con) {
		return toJson(*con);
	}

	template<typename U>
	static Json::Value toJson(const std::unique_ptr<U> & con) {
		return toJson(*con);
	}

};

template<typename T>
Json::Value toJson(const T & t) {
	return converter::toJson(t);
}

template<>
inline Json::Value toJson(const Json::Value & val) {
	return val;
}

template<>
inline Json::Value toJson(const std::string & val) {
	Json::Value ret(val);
	return ret;
}

inline Json::Value toJson(const char * val) {
	return toJson(std::string(val));
}

inline Json::Value toJson(char * val) {
	return toJson(std::string(val));
}

template<>
inline Json::Value toJson(const boost::filesystem::path & val) {
	return toJson(val.string());
}

template<>
inline Json::Value toJson(const char & val) {
	return toJson(std::string(1, val));
}

template<>
inline Json::Value toJson(const uint8_t & val) {
	Json::Value ret(val);
	return ret;
}
template<>
inline Json::Value toJson(const uint16_t & val) {
	Json::Value ret(val);
	return ret;
}
template<>
inline Json::Value toJson(const uint32_t & val) {
	Json::Value ret(val);
	return ret;
}
template<>
inline Json::Value toJson(const uint64_t & val) {
	Json::UInt64 conversion = val;
	Json::Value ret(conversion);
	return ret;
}
#ifndef __linux__
template<>
inline Json::Value toJson(const size_t & val) {
	Json::Value ret(static_cast<uint64_t>(val));
	return ret;
}
#endif
template<>
inline Json::Value toJson(const int8_t & val) {
	Json::Value ret(val);
	return ret;
}
template<>
inline Json::Value toJson(const int16_t & val) {
	Json::Value ret(val);
	return ret;
}
template<>
inline Json::Value toJson(const int32_t & val) {
	Json::Value ret(val);
	return ret;
}

template<>
inline Json::Value toJson(const int64_t & val) {
	Json::Int64 conversion = val;
	Json::Value ret(conversion);
	return ret;
}

template<>
inline Json::Value toJson(const double & val) {
	Json::Value ret(val);
	return ret;
}

template<>
inline Json::Value toJson(const bool & val) {
	Json::Value ret(val);
	return ret;
}
}
;

/**@brief Json converter, works for primitives and for classes that have toJson() const declared
 *
 * @param t The class/primitive to convert to json
 * @return Json object for t
 */
template<typename T>
Json::Value toJson(const T & t) {
	return JsonConversion::toJson(t);
}

/**@brief Parse a string into a Json::Value object
 *
 * @param jsonStr String formated in json
 * @return A Json::Value object
 */
inline Json::Value parse(const std::string & jsonStr) {
	Json::CharReaderBuilder readerBuilder;
	std::unique_ptr<Json::CharReader> const reader(readerBuilder.newCharReader());
	Json::Value root;
	std::string errs;
	auto stats = reader->parse(jsonStr.c_str(), jsonStr.c_str() + jsonStr.size(), &root, &errs);
	if (!stats) {
		std::stringstream ss;
		ss << "Error in parsing jsonStr in " << __PRETTY_FUNCTION__ << "\n";
		ss << jsonStr << "\n";
		ss << errs << "\n";
		throw std::runtime_error{ss.str()};
	}
	return root;
}

/**@brief Parse json file and create Json::Value object
 *
 * @param filename the file to  read in
 * @return a Json::Value object with the contents of filename
 */
inline Json::Value parseFile(const std::string & filename) {
	Json::CharReaderBuilder readerBuilder;
	Json::Value root;
	std::string errs;
	std::ifstream inFile{filename};
	auto stats = Json::parseFromStream(readerBuilder, inFile, &root, &errs);
	if (!stats) {
		std::stringstream ss;
		ss << "Error in parsing from file: " << filename << " in " << __PRETTY_FUNCTION__ << "\n";
		ss << errs << "\n";
		throw std::runtime_error{ss.str()};
	}
	return root;
}

/**@brief Parse a stream for json and return a Json::Value, throw on faiure
 *
 * @param is the stream to read from
 * @return the Json in the input stream
 */
inline Json::Value parseStream(std::istream & is){
	Json::CharReaderBuilder readerBuilder;
	Json::Value root;
	std::string errs;
	auto stats = Json::parseFromStream(readerBuilder, is, &root, &errs);
	if (!stats) {
		std::stringstream ss;
		ss << "Error in parsing from stream in " << __PRETTY_FUNCTION__ << "\n";
		ss << errs << "\n";
		throw std::runtime_error{ss.str()};
	}
	return root;
}

/**@brief Write a json formated string with little white space, one line
 *
 * @param val The json object
 * @return A string with only one line with values in val written in json format
 */
inline std::string writeAsOneLine(const Json::Value & val) {
	Json::StreamWriterBuilder writerBuilder;
	writerBuilder["indentation"] = "";
	return Json::writeString(writerBuilder,val);
}


/**@brief Write as one line (no indentation) to stream
 *
 * @param val the value to write
 * @param out the stream to write to
 */
inline void writeAsOneLine(const Json::Value & val, std::ostream & out) {
	Json::StreamWriterBuilder writerBuilder;
	writerBuilder.settings_["indentation"] = "";
	std::unique_ptr<Json::StreamWriter> writer(writerBuilder.newStreamWriter());
	writer->write(val, &out);
}

/**@brief convert a json array to a vector using a function to convert the json to cpp type
 *
 * @param jData the data to convert, needs to be an array
 * @param func the function used to convert
 * @return a vector of the json array
 */
template<typename T>
std::vector<T> jsonArrayToVec(const Json::Value & jData,
		const std::function<T(const Json::Value &)> & func) {
	if (!jData.isArray()) {
		std::stringstream ss;
		ss << __PRETTY_FUNCTION__ << " Error, input json data should be an array\n";
		throw std::runtime_error { ss.str() };
	}
	std::vector<T> ret(jData.size());
	std::transform(jData.begin(), jData.end(), ret.begin(), func);
	return ret;
}

/**@brief Convert a json array to a vector of strings, will throw if not an array
 *
 * @param jData the input array
 * @return a vector of the json array converted into strings
 */
inline std::vector<std::string> jsonArrayToStrVec(const Json::Value & jData) {
	return jsonArrayToVec<std::string>(jData, [](const Json::Value & jd){
		return jd.asString();
	});
}

/**@brief convert a json array to a vector using a function to convert the json to cpp type
 *
 * @param jData the data to convert, needs to be an array
 * @return a vector of the json array
 */
template<typename T>
std::set<T> jsonArrayToSet(const Json::Value & jData,
		const std::function<T(const Json::Value &)> & func) {
	/**@todo find an more efficient than to convert to vector and then set */
	auto vec = jsonArrayToVec(jData, func);
	return {vec.begin(), vec.end()};
}

/**@brief convert a json array to a set using a function to convert the json to cpp type
 *
 * @param jData the data to convert, needs to be an array
 * @return a set of the json array
 */
inline std::set<std::string> jsonArrayToStrSet(const Json::Value & jData) {
	return jsonArrayToSet<std::string>(jData, [](const Json::Value & jd){
		return jd.asString();
	});
}

} //namespace json
} //namesapce njh
