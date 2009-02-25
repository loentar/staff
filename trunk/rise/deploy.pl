#!/usr/bin/perl

use File::Find;
use File::Copy;
use File::Basename;
use POSIX qw(strftime);

$deploydir="../../deploy/win32/";

$incdir="$deploydir/include/rise";
$libdir="$deploydir/lib";

mkdir "$deploydir" || die "can't mkdir($deploydir) : $!";;

# копируем инклюды
mkdir "$deploydir/include" || die "can't mkdir($deploydir/include) : $!";;
mkdir "$incdir" || die "can't mkdir($incdir) : $!";;

#Получить список файлов каталога
sub GetTargetFileList
{
  my $CurrDir        =  shift; # текущий каталог
  my $TargetFileList =  shift; # список файлов цели

  # вернуться если не определены параметры
  return undef unless (defined($CurrDir));
  return undef unless (defined($TargetFileList));

  my $file;
  
  local *DIR;

  my (@all);

  opendir(DIR,$CurrDir) || die "Ошибка чтения каталога $CurrDir: $!\n";

  @all = readdir(DIR);

  # цикл по всем элементам каталога
  foreach $file (@all)
  {
    next if (($file eq ".") || ($file eq ".."));

    if (-d "$CurrDir\/$file")
    {
      # Запомнить каталоги
      GetTargetFileList("$CurrDir\/$file",$TargetFileList);
    }
    else
    {
      # Запомнить файлы
      # Проверка файла на шаблон
      if ($file =~ m/^*.(h|hpp)$/x)
      {
        push(@{$TargetFileList},"$CurrDir\/$file");
      }
    }
  }
  closedir(DIR);
}


#Список файлов каталога    
my $FileList = [];
my $file;

GetTargetFileList("../../rise", $FileList);

foreach $file (@{$FileList})
{
  local $outfile = $file;
  $outfile =~ s/^rise//gx;
  $outfile = "$incdir$outfile";

  local $deploydircur = dirname("$outfile");

  if( ! -d "$deploydircur" )
  {
    mkdir("$deploydircur") || die "can't mkdir($deploydircur) : $!";
  }
  print "$file -> $outfile\n";
  copy("$file", "$outfile") || die "can't copy file '$file -> $outfile': $!";
}

# копируем либы
mkdir "$libdir" || die "can't mkdir($libdir) : $!";
copy("Debug/rise.dll", "$libdir") || die "can't copy file '$file -> $outfile': $!";
copy("Debug/rise.lib", "$libdir") || die "can't copy file '$file -> $outfile': $!";

copy("../version.txt", "$deploydir/");

