#include "GenerateRecomLists.h"

GenerateRecomLists::GenerateRecomLists() {}


GenerateRecomLists::~GenerateRecomLists() {}



vector<vector<int>> GenerateRecomLists::generate_config(const vector<int>& field_list, const vector<int>& ltoken_sum_vector,
                           const vector<int>& rtoken_sum_vector, const double field_remove_ratio,
                           const uint32_t ltable_size, const uint32_t rtable_size) {
    vector<vector<int>> config_lists;
    vector<int> feat_list_copy = field_list;
    int father = 0;
    int father_index = 0;
    config_lists.push_back(feat_list_copy);

    while (feat_list_copy.size() > 0) {
        double max_ratio = 0.0;
        uint32_t ltoken_total_sum = 0, rtoken_total_sum = 0;
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

        cout << "required remove-field ratio: " << ratio << endl;
        cout << "actual max ratio: " << max_ratio << endl;
        cout << "remove field " << feat_list_copy[removed_field_index] << endl;

        for (int i = 0; i < feat_list_copy.size(); ++i) {
            vector<int> temp = feat_list_copy;
            temp.erase(temp.begin() + i);
            if (temp.size() <= 0) {
                continue;
            }
            config_lists.push_back(temp);
        }
    }

    return config_lists;
}


double double_max(const double a, double b) {
    if (a > b) {
        return a;
    }
    return b;
}
