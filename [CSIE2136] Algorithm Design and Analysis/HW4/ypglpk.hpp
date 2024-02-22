#ifndef __YPGLPK_HPP
#define __YPGLPK_HPP

#include <glpk.h>
#include <utility>
#include <vector>
#include <cassert>

// https://www.gnu.org/software/glpk/
// http://most.ccib.rutgers.edu/glpk.pdf
namespace ypglpk {
    using std::vector, std::pair;
    constexpr double INF = 1e300;

    bool output = false;

    /*
     * Set whether to output verbose information about the solver (for debugging)
     * @param _output If true, output verbose information when solving LP/MILP.
     */
    void set_output(bool _output) { output = _output; }

    /*
     * Warning: for interal usage. please do not call it unless you are really sure
     * about what you are doing
     */
    void set_constraints(glp_prob *P, const vector<vector<double>> &A,
            const vector<double> &b, const vector<double> &c) {
        const int n = static_cast<int>(c.size()), m = static_cast<int>(b.size());
        glp_set_obj_dir(P, GLP_MAX);
        glp_add_rows(P, m);
        glp_add_cols(P, n);
        for (int i = 0; i < m; i++)
            glp_set_row_bnds(P, i + 1, GLP_UP, 0, b[i]);
        for (int i = 0; i < n; i++)
            glp_set_col_bnds(P, i + 1, GLP_FR, 0, 0);
        for (int i = 0; i < n; i++)
            glp_set_obj_coef(P, i + 1, c[i]);
        vector<int> ia(1), ja(1);
        vector<double> ar(1);
        assert (static_cast<int>(A.size()) == m);
        for (int i = 0; i < m; i++) {
            assert (static_cast<int>(A[i].size()) == n);
            for (int j = 0; j < n; j++) {
                if (A[i][j] == 0)
                    continue;
                ia.push_back(i + 1);
                ja.push_back(j + 1);
                ar.push_back(A[i][j]);
            }
        }
        glp_load_matrix(P, static_cast<int>(ia.size()) - 1,
                ia.data(), ja.data(), ar.data());
    }

    /*
     * Maximize c^T x subject to Ax<=b.
     * @return pair(c^T y , y) where y is the optimal x.
     *         if the constraints are infeasible or unbounded, return pair(-INF,{}).
     */
    pair<double, vector<double>> linear_programming(const vector<vector<double>> &A,
            const vector<double> &b,
            const vector<double> &c) {
        const int n = static_cast<int>(c.size());
        glp_term_out(output ? GLP_ON : GLP_OFF);
        glp_prob *lp = glp_create_prob();
        set_constraints(lp, A, b, c);
        glp_smcp parm;
        glp_init_smcp(&parm);
        parm.presolve = GLP_ON;
        if (glp_simplex(lp, &parm)) {
            glp_delete_prob(lp);
            return {-INF, {}};
        }
        if (glp_get_status(lp) != GLP_OPT) {
            glp_delete_prob(lp);
            return {-INF, {}};
        }
        vector<double> ans(n);
        for (int i = 0; i < n; i++)
            ans[i] = glp_get_col_prim(lp, i + 1);
        const double opt = glp_get_obj_val(lp);
        glp_delete_prob(lp);
        return {opt, ans};
    }

    /*
     * Maximize c^T x subject to Ax<=b. Moreover,
     * You can restrict x[i] to be certain integral type via `vartype`
     * @param `vartype` Each term of this parameter should be one of
     *          - GLP_BV, means the i-th variable is a binary variable
     *          - GLP_IV, means the i-th variable is a integer variable
     *          - GLP_CV, means the i-th variable is a continuous variable
     * @return pair(c^T y , y) where y is the optimal x.
     *         if the constraints are infeasible or unbounded, return pair(-INF,{}).
     */
    pair<double, vector<double>> mixed_integer_linear_programming(
            const vector<vector<double>> &A, const vector<double> &b,
            const vector<double> &c, const vector<int> &vartype) {
        const int n = static_cast<int>(c.size());
        glp_term_out(output ? GLP_ON : GLP_OFF);
        glp_prob *milp = glp_create_prob();
        set_constraints(milp, A, b, c);
        for (int i = 0; i < n; i++) {
            assert (vartype[i] == GLP_BV ||
                    vartype[i] == GLP_IV ||
                    vartype[i] == GLP_CV);
            glp_set_col_kind(milp, i + 1, vartype[i]);
        }
        glp_iocp parm;
        glp_init_iocp(&parm);
        parm.presolve = GLP_ON;
        if (glp_intopt(milp, &parm)) {
            glp_delete_prob(milp);
            return {-INF, {}};
        }
        if (glp_mip_status(milp) != GLP_OPT) {
            glp_delete_prob(milp);
            return {-INF, {}};
        }
        vector<double> ans(n);
        for (int i = 0; i < n; i++)
            ans[i] = glp_mip_col_val(milp, i + 1);
        const double opt = glp_mip_obj_val(milp);
        glp_delete_prob(milp);
        return {opt, ans};
    }
} // namespace ypglpk
#endif
