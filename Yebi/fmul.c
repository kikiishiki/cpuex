#include <stdio.h>
#include <stdint.h>

uint32_t
E(uint32_t x, int i, int j) /* i and j are inclusive */
{
  return (((1 << (i + 1)) - 1) & x) >> j;
}

uint32_t
F(uint32_t x, int i)
{
  return E(x, i, i);
}

uint32_t
fmul(uint32_t x, uint32_t y)
{
  uint32_t frc_x_h, frc_y_h;
  uint32_t frc_x_l, frc_y_l;
  uint32_t exp_x1, exp_y1, exp_ans, exp_ans1, exp_ans2;
  uint32_t frc_hh, frc_hl, frc_lh, frc_result, frc_ans;
  int sgn_ans;

  frc_x_h = (1 << 12) | E(x, 22, 11);
  frc_x_l = E(x, 10, 0);
  frc_y_h = (1 << 12) | E(y, 22, 11);
  frc_y_l = E(y, 10, 0);
  exp_x1 = E(x, 30, 23);
  exp_y1 = E(y, 30, 23);
  exp_ans1 = exp_x1 + exp_y1 + 0x81;

  sgn_ans = (x >> 31) ^ (y >> 31);

  frc_hh = frc_x_h * frc_y_h;
  frc_hl = frc_x_h * frc_y_l;
  frc_lh = frc_x_l * frc_y_h;

  frc_result = frc_hh + E(frc_hl, 23, 11) + E(frc_lh, 23, 11) + 2;

  exp_ans2 = exp_ans1 + 1;

  if (F(exp_ans1, 8) == 0) {
    exp_ans = 0;
  } else if (F(frc_result, 25)) {
    exp_ans = E(exp_ans2, 7, 0);
  } else {
    exp_ans = E(exp_ans1, 7, 0);
  }

  if (F(frc_result, 25)) {
    frc_ans = E(frc_result, 24, 2);
  } else {
    frc_ans = E(frc_result, 23, 1);
  }

  if (F(exp_ans1, 8) == 0) {
    return 0;
  } else {
    return (sgn_ans << 31) + (exp_ans << 23) + frc_ans;
  }
}
