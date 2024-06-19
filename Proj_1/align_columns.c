//
// Tomás Oliveira e Silva, February 2019
//
// align (left, center, or right justify) columns of a text file
//
// assumes that the text file is encoded in utf-8
//

#include <stdio.h>
#include <string.h>

#define max_fields 1024

static struct
{
  int max_width;
  int alignment;      // -1 (left), 0 (center), or +1 (right)
  int capitalize;     // 0 (no), 1 (yes), or 2 (yes, and correct names)
  int fill_missing;   // 0 (do not fill), 1 (fill with a single "0")
  int variable_width;
  int width;
  char *text;
}
line_data[max_fields];

static void to_lower(char *text)
{
  if(*text >= 'A' && *text <= 'Z') *text += 'a' - 'A';
  if((text[0] & 0xFF) == 0xC3 && (text[1] & 0xFF) == 0x81) text[1] = 0xA1; // Á -> á
  if((text[0] & 0xFF) == 0xC3 && (text[1] & 0xFF) == 0x89) text[1] = 0xA9; // É -> é
  if((text[0] & 0xFF) == 0xC3 && (text[1] & 0xFF) == 0x8D) text[1] = 0xAD; // Í -> í
  if((text[0] & 0xFF) == 0xC3 && (text[1] & 0xFF) == 0x93) text[1] = 0xB3; // Ó -> ó
  if((text[0] & 0xFF) == 0xC3 && (text[1] & 0xFF) == 0x9A) text[1] = 0xBA; // Ú -> ú
  if((text[0] & 0xFF) == 0xC3 && (text[1] & 0xFF) == 0x80) text[1] = 0xA0; // À -> à
  if((text[0] & 0xFF) == 0xC3 && (text[1] & 0xFF) == 0x83) text[1] = 0xA3; // Ã -> ã
  if((text[0] & 0xFF) == 0xC3 && (text[1] & 0xFF) == 0x95) text[1] = 0xB5; // Õ -> õ
  if((text[0] & 0xFF) == 0xC3 && (text[1] & 0xFF) == 0x87) text[1] = 0xA7; // Ç -> ç
  if((text[0] & 0xFF) == 0xC3 && (text[1] & 0xFF) == 0x82) text[1] = 0xA2; // Â -> â
  if((text[0] & 0xFF) == 0xC3 && (text[1] & 0xFF) == 0x8A) text[1] = 0xAA; // Ê -> ê
  if((text[0] & 0xFF) == 0xC3 && (text[1] & 0xFF) == 0x94) text[1] = 0xB4; // Ô -> ô
}

static void to_upper(char *text)
{
  if(*text >= 'a' && *text <= 'z') *text += 'A' - 'a';
  if((text[0] & 0xFF) == 0xC3 && (text[1] & 0xFF) == 0xA1) text[1] = 0x81; // á -> Á
  if((text[0] & 0xFF) == 0xC3 && (text[1] & 0xFF) == 0xA9) text[1] = 0x89; // é -> É
  if((text[0] & 0xFF) == 0xC3 && (text[1] & 0xFF) == 0xAD) text[1] = 0x8D; // í -> Í
  if((text[0] & 0xFF) == 0xC3 && (text[1] & 0xFF) == 0xB3) text[1] = 0x93; // ó -> Ó
  if((text[0] & 0xFF) == 0xC3 && (text[1] & 0xFF) == 0xBA) text[1] = 0x9A; // ú -> Ú
  if((text[0] & 0xFF) == 0xC3 && (text[1] & 0xFF) == 0xA0) text[1] = 0x80; // à -> À
  if((text[0] & 0xFF) == 0xC3 && (text[1] & 0xFF) == 0xA3) text[1] = 0x83; // ã -> Ã
  if((text[0] & 0xFF) == 0xC3 && (text[1] & 0xFF) == 0xB5) text[1] = 0x95; // õ -> Õ
  if((text[0] & 0xFF) == 0xC3 && (text[1] & 0xFF) == 0xA7) text[1] = 0x87; // ç -> Ç
  if((text[0] & 0xFF) == 0xC3 && (text[1] & 0xFF) == 0xA2) text[1] = 0x82; // â -> Â
  if((text[0] & 0xFF) == 0xC3 && (text[1] & 0xFF) == 0xAA) text[1] = 0x8A; // ê -> Ê
  if((text[0] & 0xFF) == 0xC3 && (text[1] & 0xFF) == 0xB4) text[1] = 0x94; // ô -> Ô
}

static void capitalize(char *text)
{
  int i;

  while(*text != '\0')
  {
    for(i = 0;text[i] != '\0' && text[i] != ' ';i++)
    {
      if(i == 0)
        to_upper(&text[i]);
      else
        to_lower(&text[i]);
    }
    if(i == 1 && text[0] == 'E')
      to_lower(text);
    if(i == 2 && text[0] == 'D' && (text[1] == 'a' || text[1] == 'e' || text[1] == 'o'))
      to_lower(text);
    if(i == 3 && text[0] == 'D' && (text[1] == 'a' || text[1] == 'o') && text[2] == 's')
      to_lower(text);
    text += i;
    while(*text == ' ')
      text++;
  }
}

static char *correct(char *text,int stringify)
{
# define append(c) do { if(&data[out_idx] >= &buffer[sizeof(buffer)]) return NULL; data[out_idx++] = c; } while(0)
  static char buffer[16384],*data = NULL;
  static struct
  {
    char *bad;
    char *good;
  }
  corrections[] =
  {
    { "Andre", "André"   },
    { "Joao",  "João"    },
    { "Sergio","Sérgio"  },
    { "Tomas", "Tomás"   }
  };
  int in_idx,delta,out_idx,changed,i,j;

  if(data == NULL || text == NULL)
    data = buffer;
  if(text != NULL)
  {
    out_idx = 0;
    changed = 0;
    if(stringify != 0)
    {
      append('"');
      changed = 1;
    }
    for(in_idx = 0;text[in_idx] != '\0';in_idx += delta)
    {
      for(delta = 0;text[in_idx + delta] != '\0' && text[in_idx + delta] != ' ';delta++)
        ;
      if(delta == 0)
      {
        delta = 1;
        append(' ');
      }
      else
      {
        for(i = 0;i < (int)(sizeof(corrections) / sizeof(corrections[0]));i++)
          if(strncmp(&text[in_idx],corrections[i].bad,(size_t)delta) == 0)
            break;
        if(i < (int)(sizeof(corrections) / sizeof(corrections[0])))
        { // replace
          changed = 1;
          for(j = 0;corrections[i].good[j] != '\0';j++)
            append(corrections[i].good[j]);
        }
        else
        { // keep
          for(j = 0;j < delta;j++)
            append(text[in_idx + j]);
        }
      }
    }
    if(stringify != 0)
      append('"');
    append('\0');
    if(changed != 0)
    {
      text = data;
      data = &data[out_idx + 1];
    }
  }
  return text;
# undef append
}

static int split_line(char *s,char separator)
{
  int n,i,w,c,ucs,extra;

  (void)correct(NULL,0);
  //
  // remove one or more '\n' and '\r' from the end of the line
  //
  for(i = 0;s[i] != '\0';i++)
    ;
  while(i > 0 && (s[i - 1] == '\n' || s[i - 1] == '\r'))
    s[--i] = '\0';
  //
  // when the separator is a space, skip initial spaces
  //
  if(separator == ' ')
    while(*s == ' ')
      s++;
  //
  // get all fields, one at a time
  //
  for(n = 0;n < max_fields && *s != '\0';n++)
  {
    line_data[n].text = s;
    for(w = 0;*s != '\0' && *s != separator;w++)
    {
      // get next character
      c = s[0] & 0b11111111;
      if     ((c & 0b10000000) == 0b00000000) { ucs = c & 0b01111111; extra = 0; }
      else if((c & 0b11100000) == 0b11000000) { ucs = c & 0b00011111; extra = 1; }
      else if((c & 0b11110000) == 0b11100000) { ucs = c & 0b00001111; extra = 2; }
      else if((c & 0b11111000) == 0b11110000) { ucs = c & 0b00000111; extra = 3; }
      else if((c & 0b11111100) == 0b11111000) { ucs = c & 0b00000011; extra = 4; }
      else if((c & 0b11111110) == 0b11111100) { ucs = c & 0b00000001; extra = 5; }
      else return -1; // illegal UTF-8 character
      for(i = 1;i <= extra;i++)
      {
        c = s[i] & 0b11111111;
        if((c & 0b11000000) != 0b10000000)
          return -1; // illegal UTF-8 character
        ucs = (ucs << 6) + (c & 0b00111111);
      }
      s += i;
    }
    // skip separator
    if(*s == '\0')
      i = 0;
    else if(separator != ' ')
      i = 1;
    else
      for(i = 1;s[i] == ' ';i++)
        ;
    // terminate field and advance text position
    *s = '\0';
    s += i;
    // if asked, capitalize field
    if(line_data[n].capitalize > 0)
      capitalize(line_data[n].text);
    // if asked, correct field
    if(line_data[n].capitalize > 1)
    {
      line_data[n].text = correct(line_data[n].text,(line_data[n].capitalize > 2) ? 1 : 0);
      if(line_data[n].text == NULL)
        return -2; // buffer overflow
    }
    // analyze field
    for(i = 0;line_data[n].text[i] == '-';i++)
      ;
    if(i > 0 && line_data[n].text[i] == '\0')
    { // a field composed only by '-' is assumed to have variable width
      line_data[n].variable_width = 1;
      w = 1;
    }
    else
      line_data[n].variable_width = 0;
    // record width
    line_data[n].width = w;
    if(w > line_data[n].max_width)
      line_data[n].max_width = w;
  }
  for(i = n;i < max_fields;i++)
    if(line_data[i].fill_missing == 0)
    {
      line_data[i].text = "";
      line_data[i].variable_width = 0;
      line_data[i].width = 0;
    }
    else
    {
      line_data[i].text = "0";
      line_data[i].variable_width = 0;
      line_data[i].width = 1;
    }
  return n;
}

int main(int argc,char **argv)
{
  char separator,*file_name,line[16384];
  int i,j,k,nl,nr,n_fields,a;
  FILE *fp;

  //
  // defaults
  //
  for(i = 0;i < max_fields;i++)
  {
    line_data[i].max_width = 0;
    line_data[i].alignment = +1;
    line_data[i].capitalize = 0;
    line_data[i].fill_missing = 0;
  }
  separator = ' ';
  file_name = NULL;
  //
  // process command line arguments
  //
  for(i = 1;i < argc;i++)
    if(argv[i][0] == '-' && argv[i][1] == 's')
    { // separator
      if(argv[i][2] == '\0' || argv[i][3] != '\0')
        goto usage;
      switch(argv[i][2])
      {
        case ' ':
        case 's':
          separator = ' ';
          break;
        case '\t':
        case 't':
          separator = '\t';
          break;
        case '.':
        case ',':
        case ':':
        case ';':
        case '|':
        case '@':
          separator = argv[i][2];
          break;
        default:
          goto usage;
      }
    }
    else if(argv[i][0] == '-' && argv[i][1] == 'f')
    { // format
      for(j = 2;argv[i][j] != '\0' && j - 2 < max_fields;j++)
      {
        if(argv[i][j] == 'l' || argv[i][j] == 'L')
          line_data[j - 2].alignment = -1;
        else if(argv[i][j] == 'c' || argv[i][j] == 'C')
          line_data[j - 2].alignment = 0;
        else if(argv[i][j] == 'r' || argv[i][j] == 'R')
          line_data[j - 2].alignment = +1;
        else
          goto usage;
      }
      if(j == 2)
        goto usage;
      for(;j - 2 < max_fields;j++)
        line_data[j - 2].alignment = line_data[j - 3].alignment;
    }
    else if(argv[i][0] == '-' && argv[i][1] == 'c')
    { // capitalize
      if(argv[i][2] == '\0')
        goto usage;
      for(j = 2;argv[i][j] != '\0' && j - 2 < max_fields;j++)
      {
        if(argv[i][j] == 's' || argv[i][j] == 'S')
          line_data[j - 2].capitalize = 3; // capitalize, correct, and stringify
        else if(argv[i][j] == 'c' || argv[i][j] == 'C')
          line_data[j - 2].capitalize = 2; // capitalize and correct
        else if(argv[i][j] == 'y' || argv[i][j] == 'Y')
          line_data[j - 2].capitalize = 1; // capitalize
        else if(argv[i][j] == 'n' || argv[i][j] == 'N')
          line_data[j - 2].capitalize = 0; // do not capitalize
        else
          goto usage;
      }
      if(j == 2)
        goto usage;
      for(;j - 2 < max_fields;j++)
        line_data[j - 2].capitalize = line_data[j - 3].capitalize;
    }
    else if(argv[i][0] == '-' && argv[i][1] == 'm')
    { // fill missing field
      if(argv[i][2] == '\0')
        goto usage;
      for(j = 2;argv[i][j] != '\0' && j - 2 < max_fields;j++)
        if(argv[i][j] == '0')
          line_data[j - 2].fill_missing = 1; // fill a missing field with a single "0"
      if(j == 2)
        goto usage;
      for(;j - 2 < max_fields;j++)
        line_data[j - 2].fill_missing = line_data[j - 3].fill_missing;
    }
    else if(file_name == NULL)
      file_name = argv[i];
    else
    {
usage:fprintf(stderr,"usage: %s [-sseparator] [-fformat] [-cformat] [-mformat] file_name\n",argv[0]);
      fprintf(stderr,"  the character of the -s option (separator) can be\n");
      fprintf(stderr,"    ' ' or 's'\n");
      fprintf(stderr,"    '\\t' or 't'\n");
      fprintf(stderr,"    '.'\n");
      fprintf(stderr,"    ','\n");
      fprintf(stderr,"    ':'\n");
      fprintf(stderr,"    ';'\n");
      fprintf(stderr,"    '|'\n");
      fprintf(stderr,"    '@'\n");
      fprintf(stderr,"  each letter of the -f option (alignment) is one of\n");
      fprintf(stderr,"    'L' or 'l' --- align left\n");
      fprintf(stderr,"    'C' or 'c' --- align center\n");
      fprintf(stderr,"    'R' or 'r' --- align right\n");
      fprintf(stderr,"  each letter of the -c option (capitalize) is one of\n");
      fprintf(stderr,"    'S' or 's' --- capitalize, correct, and stringify\n");
      fprintf(stderr,"    'C' or 'c' --- capitalize and correct (spelling mistakes in Portuguese names)\n");
      fprintf(stderr,"    'Y' or 'y' --- capitalize\n");
      fprintf(stderr,"    'N' or 'n' --- no nothing\n");
      fprintf(stderr,"  each letter of the -m option (fill missing field) is one of\n");
      fprintf(stderr,"    '0' ----- fill a missing field with a single zero\n");
      fprintf(stderr,"    other --- do nothing\n");
      fprintf(stderr,"  the last letter of each format is used for all remaining fields\n");
      return 1;
    }
  if(file_name == NULL)
    goto usage;
  //
  // open file
  //
  fp = fopen(file_name,"rb");
  if(fp == NULL)
  {
    fprintf(stderr,"unable to open file %s\n",file_name);
    return 1;
  }
  //
  // read file
  //
  n_fields = 0;
  for(i = 1;fgets(line,sizeof(line),fp) != NULL;i++)
  {
    j = split_line(line,separator);
    if(j < 0)
    {
error:fprintf(stderr,"problem in line #%d of file %s -- %s\n",i,file_name,(j == -1) ? "bad utf8 encoding" : "buffer overflow");
      return 1;
    }
    if(j > n_fields)
      n_fields = j;
  }
  //
  // output formatted file
  //
  rewind(fp);
  for(i = 1;fgets(line,sizeof(line),fp) != NULL;i++)
  {
    j = split_line(line,separator);
    if(j < 0)
      goto error;
    for(k = 0;k < n_fields;k++)
    { // output always all columns
      if(k > 0)
        printf("%c",separator);
      if(line_data[k].variable_width != 0)
        for(a = 0;a < line_data[k].max_width;a++)
          printf("%c",line_data[k].text[0]);
      else
      {
        nl = nr = 0;
        switch(line_data[k].alignment)
        {
          case -1:
            nr = line_data[k].max_width - line_data[k].width;
            break;
          case  0:
            nl = (line_data[k].max_width - line_data[k].width) / 2;
            nr = line_data[k].max_width - line_data[k].width - nl;
            break;
          case +1:
            nl = line_data[k].max_width - line_data[k].width;
            break;
        }
        for(a = 0;a < nl;a++)
          printf(" ");
        printf("%s",line_data[k].text);
        if(k < j - 1)
          for(a = 0;a < nr;a++)
            printf(" ");
      }
    }
    printf("\n");
  }
  //
  // close file and terminate
  //
  fclose(fp);
  return 0;
}
