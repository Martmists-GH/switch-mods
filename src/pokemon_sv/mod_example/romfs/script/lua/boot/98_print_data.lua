function API:logIDs()
local exists = {}
local names = {}
local forms = {}

for i = 1, 1011 do
    if c2C618D1A.f0C9CEB5B(i, 0) ~= 0 then
        table.insert(exists, i)
        local f = {}
        forms[i] = f

        local name = gfl_string(c5776A95A.fBDB71F52(i))
        table.insert(names, name)

        local formCount = c2C618D1A.fD67612D0(i)
        for j = 0, formCount do
            if c2C618D1A.f0C9CEB5B(i, j) ~= 0 then
                table.insert(f, j)
            end
        end
    end
end

print("Valid Species:", #exists)
print(table.concat(exists, ","))
print("Species names:")
print(table.concat(names, ","))
print("Valid forms:")
for id, f in pairs(forms) do
    print(id, "=>", table.concat(f, ","))
end
end
