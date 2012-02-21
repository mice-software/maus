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

namespace MAUS {

template <class ArrayContents>
PointerArrayProcessor<ArrayContents>::PointerArrayProcessor(ProcessorBase<ArrayContents>* contents_processor) : _proc(contents_processor) {
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
        if (ProcessorBase<std::vector<ArrayContents*> >::_isStrict) {
            // no memory allocated yet...
            throw(Squeal(Squeal::recoverable, 
                        "\nFailed to resolve Json::Value to array",
                        "PointerArrayProcessor::JsonToCpp()"
                        ) );
        } else {
            return NULL;
        } 
    }
    std::vector<ArrayContents*>* vec = new std::vector<ArrayContents*>(json_array.size());

    _proc->SetIsStrict(ProcessorBase<std::vector<ArrayContents*> >::_isStrict);
    for (size_t i = 0; i < json_array.size(); ++i) {
        try {
            // allocate the vector
            ArrayContents* data = _proc->JsonToCpp(json_array[i]);
            vec->push_back(data);
        } catch (Squeal squee) {
            // if there's a problem, clean up before rethrowing the exception
            for (size_t j = 0; j < vec->size(); ++j) {
                if ((*vec)[j] != NULL) {
                    delete (*vec)[j];
                }
            }
            delete vec;
            throw squee;
        }
    }
    return vec;
}


template <class ArrayContents>
Json::Value* PointerArrayProcessor<ArrayContents>::
                      CppToJson(const std::vector<ArrayContents*>& cpp_array) {
    Json::Value* json_array = new Json::Value(Json::arrayValue);
    json_array->resize(cpp_array.size());

    _proc->SetIsStrict(ProcessorBase<std::vector<ArrayContents*> >::_isStrict);
    for (size_t i = 0; i < cpp_array.size(); ++i) {
        try {
            Json::Value* data = _proc->CppToJson(*cpp_array[i]);
            json_array[i] = *data; // json copies memory here
            delete data; // so we need to clean up here
        } catch (Squeal squee) {
            // if there's a problem, clean up before rethrowing the exception
            delete json_array;
            throw squee;
        }
    }
    return json_array;

}
///////////

template <class ArrayContents>
ValueArrayProcessor<ArrayContents>::ValueArrayProcessor(ProcessorBase<ArrayContents>* contents_processor) : _proc(contents_processor) {
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
        if (ProcessorBase<std::vector<ArrayContents> >::_isStrict) {
            // no memory allocated yet...
            throw(Squeal(Squeal::recoverable, 
                        "\nFailed to resolve Json::Value to array",
                        "ValueArrayProcessor::JsonToCpp()"
                        ) );
        } else {
            return NULL;
        } 
    }
    std::vector<ArrayContents>* vec = new std::vector<ArrayContents>(json_array.size());

    _proc->SetIsStrict(ProcessorBase<std::vector<ArrayContents> >::_isStrict);
    for (size_t i = 0; i < json_array.size(); ++i) {
        try {
            // allocate the vector
            ArrayContents* data = _proc->JsonToCpp(json_array[i]);
            vec->push_back(*data);
            delete data;
        } catch (Squeal squee) {
            // if there's a problem, clean up before rethrowing the exception
            delete vec;
            throw squee;
        }
    }
    return vec;
}


template <class ArrayContents>
Json::Value* ValueArrayProcessor<ArrayContents>::
                      CppToJson(const std::vector<ArrayContents>& cpp_array) {
    Json::Value* json_array = new Json::Value(Json::arrayValue);
    json_array->resize(cpp_array.size());

    _proc->SetIsStrict(ProcessorBase<std::vector<ArrayContents> >::_isStrict);
    for (size_t i = 0; i < cpp_array.size(); ++i) {
        try {
            Json::Value* data = _proc->CppToJson(cpp_array[i]);
            json_array[i] = *data; // json copies memory here
            delete data; // so we need to clean up here
        } catch (Squeal squee) {
            // if there's a problem, clean up before rethrowing the exception
            delete json_array;
            throw squee;
        }
    }
    return json_array;

}

}

