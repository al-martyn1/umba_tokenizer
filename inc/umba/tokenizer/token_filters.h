#pragma once

#include "filters/filter_base.h"
#include "filters/simple_pass_trough_filter.h"
#include "filters/token_collecting_filter.h"
#include "filters/unclassified_chars_collecting_filter.h"
#include "filters/simple_suffix_gluing_filter.h"
#include "filters/cc_preprocessor_filter.h"
#include "filters/simple_sequence_composing_filter.h"
#include "filters/repeated_token_composing_filter.h"
#include "filters/dbl_square_brackets_composing_filters.h"
#include "filters/identifier_to_keyword_conversion_filter.h"
#include "filters/token_range_conversion_filter.h"
#include "filters/kebab_case_composing_filter.h"

