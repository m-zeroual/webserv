#include"Cgi.hpp"

Cgi::Cgi(string ScriptName, map<String, String> scripts)
{
	PathScript = ScriptName;
	MultCGI = scripts;
}
string Cgi::HandelScript(vector<String> argvs)
{
	int			forkValeu = 0;
	int			fd[2];
	string	ext;

	ext = this->PathScript.substr(PathScript.find_last_of('.'));
	if(MultCGI[ext].size() == 0)
		return ERROR_404;
	char **argv = new char*[argvs.size() + 3];
	memset(argv, 0, (argvs.size() + 3) * sizeof(char *));
	argv[0] = (char *)MultCGI[ext].c_str();
	argv[1] = (char *)PathScript.c_str();
	for (size_t i = 0; i < argvs.size(); i++)
	{
		argv[i + 2] = (char *)argvs[i].c_str();
	}
	pipe(fd);
	forkValeu = fork();
	if(forkValeu == 0)
	{
		dup2(fd[1],STDOUT_FILENO);
		close(fd[0]);
		if(execve(argv[0], (char *const *)argv, NULL) < 0)
		{
			cout << "execve filed" << endl;
			exit(0);    
		}
	}
	waitpid(-1, 0, 0);
	String	respond;
	char res[200];
	bzero(res, 200);
	ssize_t bytes = 0;
	while((bytes = read(fd[0], res, 199)) != 0)
	{
		respond.append(res);
		bzero(res, 200);
		if (bytes < 199)
			break;
	}
	close(fd[0]);
	close(fd[1]);
	return respond;
}
Cgi::~Cgi()
{
}