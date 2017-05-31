
--[[
function Matrix(row, col)
    
    local m_mat;
    do
        for i = 1, row do
            m_mat[i] = {}
            for j = 1, col do
                -- 先填0 防止表不是数组
                m_mat[j] = 0;
            end
        end
    end
end
]]

function CreateDeterminant(n)
  local det;
  for i = 1, row do
      det[i] = {}
      for j = 1, col do
          -- 先填0 防止表不是数组
          det[j] = 0;
      end
  end
end

local function CalcDeterminant(det, n)
    if n == 2 then
        return det[1][1] * det[2][2] - det[1][2] * det[2][1];
    end
    local sum = 0;
    for j1 = 1, n do
        local M = {}
        for i2 = 2, n do
            table.insert(M, {});
            for j2 = 1, n do
                if j2 ~= j1 then
                    if j2 > j1 then
                        M[i2 - 1][j2 - 1] = det[i2][j2];
                    else
                        M[i2 - 1][j2] = det[i2][j2];
                    end
                end
            end
        end
        if (1 +j1) % 2 == 0 then
            sum = sum + CalcDeterminant(M, n - 1) * det[1][j1];
        else
            sum = sum - CalcDeterminant(M, n - 1) * det[1][j1];
        end
    end
    return sum;
end

local D0 =
{
{ 200, 0,  0},
{ 12,  0,  50},
{ 128, 11, 4},
}
local d0 = CalcDeterminant(D0, 3);
print (d0);

local D1 =
{
{ 1, 0,  0},
{ 0,  0,  50},
{ 0.9, 11, 4},
}
local d1 = CalcDeterminant(D1, 3);
print (d1);

local D2 =
{
{ 200, 1,  0},
{ 12,  0,  50},
{ 128, 0.9, 4},
}
local d2 = CalcDeterminant(D2, 3)
print (d2);

local D3 =
{
{ 200, 0,  1},
{ 12,  0,  0},
{ 128, 11, 0.9},
}
local d3 = CalcDeterminant(D3, 3)
print (d3);
local ratio = 1000;
print (d1/d0 * ratio, d2/d0 * ratio, d3/d0 * ratio);