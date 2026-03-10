#pragma once

typedef struct MimeType {
	char *extension;
	char *mime_type;
} mime_type_t;

const mime_type_t mime_types[] =
{
	{".txt",  "text/plain"},
	{".html", "text/html"},
	{".htm",  "text/html"},
	{".css",  "text/css"},
	{".js",   "application/javascript"},
	{".jpg",  "image/jpeg"},
	{".jpeg", "image/jpeg"},
	{".png",  "image/png"},
	{".gif",  "image/gif"},
	{".bmp",  "image/bmp"},
	{".pdf",  "application/pdf"},
	{".doc",  "application/msword"},
	{".docx", "application/vnd.openxmlformats-officedocument.wordprocessingml.document"},
	{".xls",  "application/vnd.ms-excel"},
	{".xlsx", "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet"},
	{".zip",  "application/zip"},
	{".gz",   "application/gzip"},
	{".tar",  "application/x-tar"},
	{".mp3",  "audio/mpeg"},
	{".wav",  "audio/wav"},
	{".mp4",  "video/mp4"},
	{".mov",  "video/quicktime"},
	{".avi",  "video/x-msvideo"},
	{NULL, NULL}
};

char *get_extension(char *filename)
{
	char *ext = strrchr(filename, '.');
	if (ext == NULL)
	{
		return NULL;
	}

	return ext;
}

char *get_mime_type(char *filename)
{
	char *ext = get_extension(filename);

	if (ext == NULL)
	{
		return "application/octet-stream";
	}

	for (int i = 0; i < sizeof(mime_types) / sizeof(mime_types[0]); i++)
	{
		if (mime_types[i].extension == NULL) continue;
		if (strcmp(mime_types[i].extension, ext) == 0)
		{
			return mime_types[i].mime_type;
		}
	}

	return "application/octet-stream";
}

size_t get_file_length(char *filename)
{
	FILE *fptr = fopen(filename, "r");
	if (fptr == NULL)
	{
		return 0;
	}

	size_t length = 0;

	while (fgetc(fptr) != EOF)
	{
		length++;
	}

	fclose(fptr);

	return length;
}

