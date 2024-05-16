#pragma once

#include <nlohmann/json_fwd.hpp>

//template<typename T, typename AllocatorType>
//class q_vector_wrapper<T, AllocatorType> : public QVector<T>
//{};

using ordered_json = nlohmann::basic_json<nlohmann::ordered_map, std::vector/*, QString, bool, int, unsigned int, double*/>;

