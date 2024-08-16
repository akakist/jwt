#!/usr/bin/perl

@a=<>;
my %arr;
print $ARGV[1]."\n";
foreach(@a)
{
    chomp();
#    print($_."\n");
    open(FH, '<', $_) or die $!;
    while(<FH>)
    {
        chomp;
	if(/genum_(\w+)/g)
	{
#    	    print $1."\n";
	    $arr{$1}=1;
#    	    push(@arr,$1);
    	}
    }
    close(FH);
    
    open(H,'>',"megatron/genum.hpp");
    print H "#pragma once\n";
    print H "enum genum\n{\n";
    foreach(keys %arr)
    {
	print H "\tgenum_".$_,",\n";
    }
    print H "};\n";
#    print H "const char* gen_string(int n);\n";
    
    
#    close(H);


#    open(C,'>',"genum.cpp");
    print H "#include <stdio.h>\n";
#    print C "#include \"genum.hpp\"\n";
    print H "inline const char* __gen_string123(int n){\n";
    print H "switch(n){\n";
    foreach(keys %arr)
    {
	print H "case genum_$_: return \"$_\";\n";    
    }
	print H "default: return \"undef\";\n";    
    
    print H "}\n";
    print H "return \"undef\";\n";
    
    print H "}\n";
    close(H);

}