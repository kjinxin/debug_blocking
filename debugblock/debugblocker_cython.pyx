from libcpp.vector cimport vector
from libcpp.set cimport set as cset
from libcpp.map cimport map as cmap
from libcpp.queue cimport priority_queue as heap
from libcpp.string cimport string
from libcpp cimport bool
from libc.stdio cimport printf, fprintf, fopen, fclose, FILE, sprintf
#from libcpp.stdint cimport uint32_t as uint
from cython.parallel import prange, parallel
import time
ctypedef unsigned int uint

cdef extern from "GenerateRecomLists.h" nogil:
    cdef cppclass GenerateRecomLists:
        GenerateRecomLists()
        vector[vector[int]]  generate_config(vector[int]& field_list, vector[int]& ltoken_sum_vector, 
                              vector[int]& rtoken_sum_vector, double field_remove_ratio, int lvector_size, int rvector_size);

PREFIX_MATCH_MAX_SIZE = 5
REC_AVE_LEN_THRES = 20
OFFSET_OF_FIELD_NUM = 10
MINIMAL_NUM_FIELDS = 0
FIELD_REMOVE_RATIO = 0.1

def debugblocker_cython(lrecord_token_list, rrecord_token_list,
                        lrecord_index_list, rrecord_index_list,
                        lrecord_field_list, rrecord_field_list,
                        ltable_field_token_sum, rtable_field_token_sum, py_cand_set,
                        py_num_fields, py_output_size, py_output_path, py_use_plain,
                        py_use_new_topk, py_use_parallel):
    cdef string output_path = py_output_path
    cdef bool use_plain = py_use_plain
    cdef bool use_new_topk = py_use_new_topk
    cdef bool use_parallel = py_use_parallel

    ### Convert py objs to c++ objs
    cdef vector[vector[int]] ltoken_vector, rtoken_vector
    convert_table_to_vector(lrecord_token_list, ltoken_vector)
    convert_table_to_vector(rrecord_token_list, rtoken_vector)

    cdef vector[vector[int]] lindex_vector, rindex_vector
    convert_table_to_vector(lrecord_index_list, lindex_vector)
    convert_table_to_vector(rrecord_index_list, rindex_vector)

    cdef vector[vector[int]] lfield_vector, rfield_vector
    convert_table_to_vector(lrecord_field_list, lfield_vector)
    convert_table_to_vector(rrecord_field_list, rfield_vector)

    cdef vector[int] ltoken_sum, rtoken_sum
    convert_py_list_to_vector(ltable_field_token_sum, ltoken_sum)
    convert_py_list_to_vector(rtable_field_token_sum, rtoken_sum)

    cdef cmap[int, cset[int]] cand_set
    convert_candidate_set_to_c_map(py_cand_set, cand_set)

    cdef vector[int] field_list
    for i in range(py_num_fields):
        field_list.push_back(i)

    cdef uint output_size = py_output_size
    cdef uint prefix_match_max_size = PREFIX_MATCH_MAX_SIZE
    cdef uint rec_ave_len_thres = REC_AVE_LEN_THRES
    cdef uint offset_of_field_num = OFFSET_OF_FIELD_NUM
    cdef uint minimal_num_fields = MINIMAL_NUM_FIELDS
    cdef double field_remove_ratio = FIELD_REMOVE_RATIO

    del lrecord_token_list, rrecord_token_list
    del lrecord_index_list, rrecord_index_list
    del lrecord_field_list, rrecord_field_list
    del py_cand_set


    cdef int max_field_num = field_list.size()

    cdef GenerateRecomLists generator

    cdef vector[vector[int]] config_vector
    config_vector = generator.generate_config(field_list, ltoken_sum, rtoken_sum, field_remove_ratio, ltoken_vector.size(), rtoken_vector.size())
    for i in xrange(config_vector.size()):
        for j in xrange(config_vector[i].size()):
            printf("%d ", config_vector[i][j]);
        printf("\n");





cdef void copy_table_and_remove_field(const vector[vector[int]]& table_vector,
                                      const vector[vector[int]]& index_vector,
                                      vector[vector[int]]& new_table_vector, int rm_field):
    cdef uint i, j
    for i in xrange(table_vector.size()):
        new_table_vector.push_back(vector[int]())
        for j in xrange(table_vector[i].size()):
            if index_vector[i][j] != rm_field:
                new_table_vector[i].push_back(table_vector[i][j])


cdef void remove_field(vector[vector[int]]& table_vector,
                       vector[vector[int]]& index_vector, int rm_field):
    cdef uint i, j
    for i in xrange(table_vector.size()):
        for j in reversed(range(table_vector[i].size())):
            if index_vector[i][j] == rm_field:
                index_vector[i].erase(index_vector[i].begin() + j)
                table_vector[i].erase(table_vector[i].begin() + j)


cdef void convert_table_to_vector(table_list, vector[vector[int]]& table_vector):
    cdef int i, j
    for i in range(len(table_list)):
        table_vector.push_back(vector[int]())
        for j in range(len(table_list[i])):
            table_vector[i].push_back(table_list[i][j])


cdef void convert_candidate_set_to_c_map(cand_set, cmap[int, cset[int]]& new_set):
    cdef int key, value
    for key in cand_set:
        if not new_set.count(key):
            new_set[key] = cset[int]()

        l = cand_set[key]
        for value in l:
            new_set[key].insert(value)


cdef int convert_py_list_to_vector(py_list, vector[int]& vector):
    for value in py_list:
        vector.push_back(value)



