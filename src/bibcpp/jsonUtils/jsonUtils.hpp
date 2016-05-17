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
#include <deque>
#include <string>
#include <cstdint>
#include <iostream>
#include "bibcpp/common.h" //Enable_if
#include "bibcpp/utils/has_member.hpp" //has_member
#include "bibcpp/debug/exception.hpp" //Exception



namespace bib{
/**@brief Namespace for dealing with conversion to json
 *
 */
namespace json{


/**@brief Namepsace to hold all conversion of objects/primitive to json
 *
 */
namespace JsonConversion{

template<typename T>
Json::Value toJson(const T & t);

/**@brief helper func to add the json of a pair to another json so key to value works properly for maps
 *
 * @param pair The pair to convert with the first member is the key and the second is the value
 * @param ret The json object to add to
 * @return A ref to the json object given by ret but with the json added for this pair
 */
template<typename FIRST, typename SECOND>
static Json::Value & toJsonHelper(const std::pair<FIRST,SECOND> & pair, Json::Value & ret){
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
static Json::Value & toJsonHelper(const VAL & val, Json::Value & ret){
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
		static estd::Enable_if<!class_has_toJson<U>::value, Json::Value> toJson(const U & con){
			Json::Value ret;
			for(const auto & val : con){
				toJsonHelper(val,ret);
			}
			if(ret.isNull()){
				ret = "";
			}
			return ret;
		}

		template<typename U>
		static estd::Enable_if<class_has_toJson<U>::value, Json::Value> toJson(const U & con){
			return con.toJson();
		}

		template<typename U>
		static Json::Value toJson(const std::shared_ptr<U> & con){
			return toJson(*con);
		}

		template<typename U>
		static Json::Value toJson(const std::unique_ptr<U> & con){
			return toJson(*con);
		}

	};

	template<typename T>
	Json::Value toJson(const T & t){
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

	inline Json::Value toJson(const char * val){
		return toJson(std::string(val));
	}

	inline Json::Value toJson(char * val){
		return toJson(std::string(val));
	}

	template<>
	inline Json::Value toJson(const char & val){
		return toJson(std::string(1,val));
	}

	template<>
	inline Json::Value toJson(const uint8_t & val){
		Json::Value ret(val);
		return ret;
	}
	template<>
	inline Json::Value toJson(const uint16_t & val){
		Json::Value ret(val);
		return ret;
	}
	template<>
	inline Json::Value toJson(const uint32_t & val){
		Json::Value ret(val);
		return ret;
	}
	template<>
	inline Json::Value toJson(const uint64_t & val){
		Json::UInt64 conversion = val;
		Json::Value ret(conversion);
		return ret;
	}
	#ifndef __linux__
	template<>
	inline Json::Value toJson(const size_t & val){
		Json::Value ret(static_cast<uint64_t>(val));
		return ret;
	}
	#endif
	template<>
	inline Json::Value toJson(const int8_t & val){
		Json::Value ret(val);
		return ret;
	}
	template<>
	inline Json::Value toJson(const int16_t & val){
		Json::Value ret(val);
		return ret;
	}
	template<>
	inline Json::Value toJson(const int32_t & val){
		Json::Value ret(val);
		return ret;
	}

	template<>
	inline Json::Value toJson(const int64_t & val){
		Json::Int64 conversion = val;
		Json::Value ret(conversion);
		return ret;
	}

	template<>
	inline Json::Value toJson(const double & val){
		Json::Value ret(val);
		return ret;
	}

	template<>
	inline Json::Value toJson(const bool & val){
		Json::Value ret(val);
		return ret;
	}
};



/**@brief Json converter, works for primitives and for classes that have toJson() const declared
 *
 * @param t The class/primitive to convert to json
 * @return Json object for t
 */
template<typename T>
Json::Value toJson(const T & t){
	return JsonConversion::toJson(t);
}


/**@brief Parse a string into a Json::Value object
 *
 * @param jsonStr String formated in json
 * @return A Json::Value object
 */
inline Json::Value parse(const std::string & jsonStr){
	Json::Reader reader;
	Json::Value root;
	auto stats =  reader.parse(jsonStr,root);
	if(!stats){
		std::cerr << "Error in parsing jsonStr for readObjectIOOptions in " << __PRETTY_FUNCTION__ << std::endl;
		std::cerr << jsonStr << std::endl;
		throw bib::err::Exception(bib::err::F() << "Could not construct from jsonStr");
	}
	return root;
}

/**@brief Write a json formated string with little white space, one line
 *
 * @param val The json object
 * @return A string with only one line with values in val written in json format
 */
inline std::string writeAsOneLine(const Json::Value & val){
	Json::FastWriter jWriter;
	return jWriter.write(val);
}




}//namespace json
}//namesapce bib
