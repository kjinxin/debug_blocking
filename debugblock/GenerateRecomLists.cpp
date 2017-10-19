#include "GenerateRecomLists.h"
GenerateRecomLists::GenerateRecomLists() {}


GenerateRecomLists::~GenerateRecomLists() {}

void inline copy_table_and_remove_fields(const vector<int>& config, const Table& table_vector, const Table& index_vector,
                                         Table& new_table_vector, Table& new_index_vector) {
    set<int> field_set;
    for (int i = 0; i < config.size(); i ++) {
        field_set.insert(config[i]);
    }

    for (int i = 0; i < table_vector.size(); ++i) {
        new_table_vector.push_back(vector<int> ());
        new_index_vector.push_back(vector<int> ());
        for (int j = 0; j < table_vector[i].size(); ++j) {
            if (field_set.count(index_vector[i][j])) {
                new_table_vector[i].push_back(table_vector[i][j]);
                new_index_vector[i].push_back(table_vector[i][j]);
            }
        }
    }
}

void GenerateRecomLists::generate_recom_lists(
                              Table& ltoken_vector, Table& rtoken_vector,
                              Table& lindex_vector, Table& rindex_vector,
                              Table& lfield_vector, Table& rfield_vector,
                              vector<int>& ltoken_sum_vector, vector<int>& rtoken_sum_vector, vector<int>& field_list,
                              CandSet& cand_set, unsigned int prefix_match_max_size, unsigned int rec_ave_len_thres,
                              unsigned int offset_of_field_num, unsigned int max_field_num,
                              unsigned int minimal_num_fields, double field_remove_ratio,
                              unsigned int output_size) {

    Table config_lists = generate_config(field_list, ltoken_sum_vector, rtoken_sum_vector, field_remove_ratio,
                           ltoken_vector.size(), rtoken_vector.size()); 

    int config_num = config_lists.size();
    vector<Heap> rec_lists(config_num);
    for (int i = 0; i < config_num; i ++) {
      Table new_ltoken_vector, new_rtoken_vector, new_lindex_vector, new_rindex_vector;
      copy_table_and_remove_fields(config_lists[i], ltoken_vector, lindex_vector, new_ltoken_vector, new_lindex_vector);
      copy_table_and_remove_fields(config_lists[i], rtoken_vector, rindex_vector, new_rtoken_vector, new_rindex_vector);
      rec_lists[i] = original_topk_sim_join_plain(new_ltoken_vector, new_rtoken_vector, cand_set, output_size);
      Heap tmp = rec_lists[i];
      cout << i << ' ' << tmp.size() << endl;
      while(!tmp.empty()) {
        //cout << tmp.top().l_rec << ' ' << tmp.top().r_rec << ' ' << tmp.top().sim << endl;
        tmp.pop();
      }
    }
}

vector<TopPair> GenerateRecomLists::merge_topk_lists(vector<Heap>& rec_lists) {
  vector<TopPair> rec_list;

  return rec_list;
}

Table GenerateRecomLists::generate_config(const vector<int>& field_list, const vector<int>& ltoken_sum_vector,
                           const vector<int>& rtoken_sum_vector, const double field_remove_ratio,
                           const unsigned int ltable_size, const unsigned int rtable_size) {
    Table config_lists;
    vector<int> feat_list_copy = field_list;
    int father = 0;
    int father_index = 0;
    config_lists.push_back(feat_list_copy);
    cout << config_lists.size() << endl;   
    while (feat_list_copy.size() > 1) {
        double max_ratio = 0.0;
        unsigned int ltoken_total_sum = 0, rtoken_total_sum = 0;
        int removed_field_index = -1;
        bool has_long_field = false;

        for (int i = 0; i < feat_list_copy.size(); ++i) {
            ltoken_total_sum += ltoken_sum_vector[feat_list_copy[i]];
            rtoken_total_sum += rtoken_sum_vector[feat_list_copy[i]];
        }

        double lrec_ave_len = ltoken_total_sum * 1.0 / ltable_size;
        double rrec_ave_len = rtoken_total_sum * 1.0 / rtable_size;
        double ratio = 1 - (feat_list_copy.size() - 1) * field_remove_ratio / (1.0 + field_remove_ratio) *
                 double_max(lrec_ave_len, rrec_ave_len) / (lrec_ave_len + rrec_ave_len);

        for (int i = 0; i < feat_list_copy.size(); ++i) {
            max_ratio = double_max(max_ratio, double_max(ltoken_sum_vector[feat_list_copy[i]] * 1.0 / ltoken_total_sum,
                                                         rtoken_sum_vector[feat_list_copy[i]] * 1.0 / rtoken_total_sum));
            if (ltoken_sum_vector[feat_list_copy[i]] > ltoken_total_sum * ratio ||
                    rtoken_sum_vector[feat_list_copy[i]] > rtoken_total_sum * ratio) {
                removed_field_index = i;
                has_long_field = true;
                break;
            }
        }

        if (removed_field_index < 0) {
            removed_field_index = feat_list_copy.size() - 1;
        }

        //cout << "required remove-field ratio: " << ratio << endl;
        //cout << "actual max ratio: " << max_ratio << endl;
        //cout << "remove field " << feat_list_copy[removed_field_index] << endl;

        for (int i = 0; i < feat_list_copy.size(); ++i) {
            vector<int> temp = feat_list_copy;
            temp.erase(temp.begin() + i);
            if (temp.size() <= 0) {
                continue;
            }
            config_lists.push_back(temp);
        }
        feat_list_copy.erase(feat_list_copy.begin() + removed_field_index);
    }

    return config_lists;
}


double double_max(const double a, double b) {
    if (a > b) {
        return a;
    }
    return b;
}
