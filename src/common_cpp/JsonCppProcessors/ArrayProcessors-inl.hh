/* This file is part of MAUS: http://micewww.pp.rl.ac.uk/projects/maus
 *
 * MAUS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * MAUS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with MAUS.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <string>
#include <vector>

#include "src/common_cpp/Utils/JsonWrapper.hh"
#include "src/common_cpp/JsonCppProcessors/Common/ReferenceResolverCppToJson.hh"
#include "src/common_cpp/JsonCppProcessors/Common/ReferenceResolverJsonToCpp.hh"

namespace MAUS {

template <class ArrayContents>
PointerArrayProcessor<ArrayContents>::PointerArrayProcessor
                            (ProcessorBase<ArrayContents>* contents_processor)
                                                   : _proc(contents_processor) {
}

template <class ArrayContents>
PointerArrayProcessor<ArrayContents>::~PointerArrayProcessor<ArrayContents>() {
    if (_proc != NULL) {
        delete _proc;
        _proc = NULL;
    }
}

template <class ArrayContents>
std::vector<ArrayContents*>* PointerArrayProcessor<ArrayContents>::JsonToCpp
                                               (const Json::Value& json_array) {
    if (!json_array.isConvertibleTo(Json::arrayValue)) {
        // no memory allocated yet...
        throw(Exceptions::Exception(Exceptions::recoverable,
                    "Failed to resolve Json::Value of type "+
                    JsonWrapper::ValueTypeToString(json_array.type())+
                    " to array",
                    "PointerArrayProcessor::JsonToCpp()"
                    ) );
    }
    std::vector<ArrayContents*>* vec = new std::vector<ArrayContents*>(json_array.size());
    for (size_t i = 0; i < json_array.size(); ++i) {
        try {
            // allocate the vector
            if (json_array[Json::Value::ArrayIndex(i)].type() == Json::nullValue) {
                (*vec)[i] = NULL;
            } else {
                ArrayContents* data = _proc->JsonToCpp(
                                      json_array[Json::Value::ArrayIndex(i)]);
                (*vec)[i] = data;
                using ReferenceResolver::JsonToCpp::RefManager;
                std::string path = JsonWrapper::Path::GetPath(
                                        json_array[Json::Value::ArrayIndex(i)]);
                if (RefManager::HasInstance())
                    RefManager::GetInstance().SetPointerAsValue(path, (*vec)[i]);
            }
        } catch (Exceptions::Exception exc) {
            // if there's a problem, clean up before rethrowing the exception
            for (size_t j = 0; j < vec->size(); ++j) {
                if ((*vec)[j] != NULL) {
                    delete (*vec)[j];
                }
            }
            delete vec;
            throw exc;
        }
    }
    return vec;
}

template <class ArrayContents>
Json::Value* PointerArrayProcessor<ArrayContents>::
                      CppToJson(const std::vector<ArrayContents*>& cpp_array) {
    return CppToJson(cpp_array, "");
}

template <class ArrayContents>
Json::Value* PointerArrayProcessor<ArrayContents>::
                      CppToJson(const std::vector<ArrayContents*>& cpp_array,
                                std::string path) {
    Json::Value* json_array = new Json::Value(Json::arrayValue);
    JsonWrapper::Path::SetPath(*json_array, path);
    json_array->resize(cpp_array.size());

    for (size_t i = 0; i < cpp_array.size(); ++i) {
        try {
            Json::Value* data = NULL;
            if (cpp_array[i] == NULL) {
                data = new Json::Value();  // that is a NULL value
            } else {
                data = _proc->CppToJson(*cpp_array[i], GetPath(path, i));
            }
            // json copies memory here but not path
            (*json_array)[Json::Value::ArrayIndex(i)] = *data;
            JsonWrapper::Path::SetPath((*json_array)[Json::Value::ArrayIndex(i)],
                                       GetPath(path, i));
            delete data; // so we need to clean up here
            using ReferenceResolver::CppToJson::RefManager;
            if (RefManager::HasInstance())
                RefManager::GetInstance().SetPointerAsValue
                                               (cpp_array[i], GetPath(path, i));
        } catch (Exceptions::Exception exc) {
            // if there's a problem, clean up before rethrowing the exception
            delete json_array;
            throw exc;
        }
    }
    return json_array;
}

template <class ArrayContents>
std::string PointerArrayProcessor<ArrayContents>::GetPath
                                              (std::string path, size_t index) {
    return path+"/"+STLUtils::ToString(index);
}
///////////

template <class ArrayContents>
ValueArrayProcessor<ArrayContents>::ValueArrayProcessor
                              (ProcessorBase<ArrayContents>* contents_processor)
                                                   : _proc(contents_processor) {
}

template <class ArrayContents>
ValueArrayProcessor<ArrayContents>::~ValueArrayProcessor<ArrayContents>() {
    if (_proc != NULL) {
        delete _proc;
        _proc = NULL;
    }
}


template <class ArrayContents>
std::vector<ArrayContents>* ValueArrayProcessor<ArrayContents>::JsonToCpp
                                               (const Json::Value& json_array) {
    if (!json_array.isConvertibleTo(Json::arrayValue)) {
        // no memory allocated yet...
        throw(Exceptions::Exception(Exceptions::recoverable,
                    "Failed to resolve Json::Value of type "+
                    JsonWrapper::ValueTypeToString(json_array.type())+
                    " to array",
                    "ValueArrayProcessor::JsonToCpp()"
                    ) );
    }
    std::vector<ArrayContents>* vec
                            = new std::vector<ArrayContents>(json_array.size());

    for (size_t i = 0; i < json_array.size(); ++i) {
        try {
            // allocate the vector
            ArrayContents* data = _proc->JsonToCpp(json_array[
                                      Json::Value::ArrayIndex(i)]);
            (*vec)[i] = *data;
            delete data;
        } catch (Exceptions::Exception exc) {
            // if there's a problem, clean up before rethrowing the exception
            delete vec;
            throw exc;
        }
    }
    return vec;
}


template <class ArrayContents>
Json::Value* ValueArrayProcessor<ArrayContents>::
                      CppToJson(const std::vector<ArrayContents>& cpp_array) {
    return CppToJson(cpp_array, "");
}

template <class ArrayContents>
Json::Value* ValueArrayProcessor<ArrayContents>::
                      CppToJson(const std::vector<ArrayContents>& cpp_array,
                                std::string path) {
    Json::Value* json_array = new Json::Value(Json::arrayValue);
    JsonWrapper::Path::SetPath(*json_array, path);
    json_array->resize(cpp_array.size());

    for (size_t i = 0; i < cpp_array.size(); ++i) {
        try {
            Json::Value* data = _proc->CppToJson(cpp_array[i],
                                                              GetPath(path, i));
            (*json_array)[Json::Value::ArrayIndex(i)] = *data; // json copies memory but not path
            JsonWrapper::Path::SetPath((*json_array)[Json::Value::ArrayIndex(i)],
                                       GetPath(path, i));
            delete data; // so we need to clean up here
        } catch (Exceptions::Exception exc) {
            // if there's a problem, clean up before rethrowing the exception
            delete json_array;
            throw exc;
        }
    }
    return json_array;
}

template <class ArrayContents>
std::string ValueArrayProcessor<ArrayContents>::GetPath
                                              (std::string path, size_t index) {
    return path+"/"+STLUtils::ToString(index);
}

////////////////////////////
template <class ArrayContents>
std::vector<ArrayContents*>* ReferenceArrayProcessor<ArrayContents>::JsonToCpp
                                               (const Json::Value& json_array) {
    using ReferenceResolver::JsonToCpp::RefManager;
    using ReferenceResolver::JsonToCpp::VectorResolver;
    if (!json_array.isConvertibleTo(Json::arrayValue)) {
        // no memory allocated yet...
        throw(Exceptions::Exception(Exceptions::recoverable,
                    "Failed to resolve Json::Value of type "+
                    JsonWrapper::ValueTypeToString(json_array.type())+
                    " to array",
                    "ValueArrayProcessor::JsonToCpp()"
                    ) );
    }
    std::vector<ArrayContents*>* vec
                     = new std::vector<ArrayContents*>(json_array.size(), NULL);

    for (size_t i = 0; i < json_array.size(); ++i) {
        try {
            (*vec)[i] = NULL;
            if (json_array[Json::Value::ArrayIndex(i)].type() != Json::nullValue) {
                std::string data_path = JsonWrapper::GetProperty
                   (json_array[Json::Value::ArrayIndex(i)],
                    "$ref",
                    JsonWrapper::stringValue).asString();
                // allocate the vector
                if (RefManager::HasInstance()) {
                    VectorResolver<ArrayContents>* res =
                          new VectorResolver<ArrayContents>(data_path, vec, i);
                    RefManager::GetInstance().AddReference(res);
                }
            }
        } catch (Exceptions::Exception exc) {
            // if there's a problem, clean up before rethrowing the exception
            delete vec;
            throw exc;
        }
    }
    return vec;
}

template <class ArrayContents>
Json::Value* ReferenceArrayProcessor<ArrayContents>::CppToJson(
                           const std::vector<ArrayContents*>& cpp_array,
                           std::string path) {
    using ReferenceResolver::CppToJson::RefManager;
    using ReferenceResolver::CppToJson::TypedResolver;
    Json::Value* array = new Json::Value(Json::arrayValue);
    JsonWrapper::Path::SetPath(*array, path);
    array->resize(cpp_array.size());
    for (size_t i = 0; i < cpp_array.size(); ++i) {
        (*array)[Json::Value::ArrayIndex(i)] = Json::Value();
        if (cpp_array[Json::Value::ArrayIndex(i)] != NULL) {
            JsonWrapper::Path::SetPath((*array)[Json::Value::ArrayIndex(i)], path);
            JsonWrapper::Path::AppendPath((*array)[Json::Value::ArrayIndex(i)], i);
            if (RefManager::HasInstance()) {
                std::string arr_path = JsonWrapper::Path::GetPath(
                                          (*array)[Json::Value::ArrayIndex(i)]);
                TypedResolver<ArrayContents>* res = new TypedResolver
                                        <ArrayContents>(cpp_array[i], arr_path);
                RefManager::GetInstance().AddReference(res);
            }
        }
    }
    return array;
}
}

