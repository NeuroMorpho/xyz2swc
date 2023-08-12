function result = extractAfter(str, pattern)

n = regexp(str,pattern);

if length(n) == 0
    result = NaN;
    return;
else
    n = n(1);
    result = substr(str,n+1);
    return;
end