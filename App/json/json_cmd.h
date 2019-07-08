#include "json.h"
#include <string>
#include <vector>
#include "../protocol/common.pb.h"
#ifndef _JSON_CMD_H_
#define _JSON_CMD_H_
// /////////////////////////////////////
struct vtdu_play_req
{
	std::string dev_id;
	int  result;
	int	 stream_type;
	int  trans_mode;
	int  dev_line;
	std::string token;
	int channel;
public:
	std::string toJson()
	{
		Json::Value root;
		root["dev_id"] = dev_id.c_str();
		root["token"] = token.c_str();
		root["channel"] = channel;
		root["stream_type"] = stream_type;
		root["trans_mode"] = trans_mode;
		root["dev_line"] = dev_line;
		return root.toStyledString();
	}

	bool jsonparse(std::string strJson)
	{
		Json::Reader reader;  
		// Json::Value是一种很重要的类型，可以代表任意类型。如int, string, object, array...  
		Json::Value root;         

		if (reader.parse(strJson.c_str(), root))  
		{
			dev_id = root["dev_id"].asCString();
			token = root["token"].asCString();
			channel = root["channel"].asInt();
			stream_type = root["stream_type"].asInt();
			trans_mode = root["trans_mode"].asInt();
			dev_line = root["dev_line"].asInt();
		}
		return true;
	}
};
struct vtdu_play_rsp
{
	std::string dev_id;
	int  result;
	int	 channel;
	std::string token;
	std::string sess_id;
public:
	std::string toJson()
	{
		Json::Value root;
		root["dev_id"] = dev_id.c_str();
		root["token"] = token.c_str();
		root["channel"] = channel;
		root["result"] = result;
		root["sess_id"] = sess_id.c_str();
		return root.toStyledString();
	}

	bool jsonparse(std::string strJson)
	{
		Json::Reader reader;  
		// Json::Value是一种很重要的类型，可以代表任意类型。如int, string, object, array...  
		Json::Value root;         

		if (reader.parse(strJson.c_str(), root))  
		{
			dev_id = root["dev_id"].asCString();
			token = root["token"].asCString();
			channel = root["channel"].asInt();
			result = root["result"].asInt();
			sess_id = root["sess_id"].asCString();
		}
		return true;
	}

};

//////////////////////////////////////////////
//////////////////////////////////////////////
//////////////////////////////////////////////
//////////////////////////////////////////////
//////////////////////////////////////////////
//////////////////////////////////////////////
//////////////////////////////////////////////
//////////////////////////////////////////////
//////////////////////////////////////////////
//////////////////////////////////////////////
//////////////////////////////////////////////
//////////////////////////////////////////////
//////////////////////////////////////////////
//////////////////////////////////////////////
struct http_json_req
{
	int cmd_type;
	http_json_req()
	{
		cmd_type = 0;
	}
	std::string toJson()
	{
		return "";
	}

	bool jsonparse(std::string strJson)
	{
		Json::Reader reader;  
		// Json::Value是一种很重要的类型，可以代表任意类型。如int, string, object, array...  
		Json::Value root;         
		if (reader.parse(strJson.c_str(), root))  
		{
			cmd_type = root["cmd_type"].asInt();
		}
		return true;
	}

};

struct http_json_rsp
{
	int result;
	http_json_rsp()
	{
		result = 0;
	}
	std::string toJson()
	{
		return "";
	}

	bool jsonparse(std::string strJson)
	{
		Json::Reader reader;  
		// Json::Value是一种很重要的类型，可以代表任意类型。如int, string, object, array...  
		Json::Value root;         
		if (reader.parse(strJson.c_str(), root))  
		{
			result = root["result"].asInt();
		}
		return true;
	}

};
struct dag_play_req:http_json_req
{
	std::string dev_id;
	int  channel;
	int	 stream_type;
	int  trans_mode;
	std::string token;
	std::string vsip;
	int vsport;
	dag_play_req()
	{
		channel = 1;
		stream_type = 0; 
		trans_mode = 0;
		cmd_type = wmp::CMD_DEVICE_PLAY;
	}
public:
	std::string toJson()
	{
		Json::Value root;
		root["dev_id"] = dev_id.c_str();
		root["token"] = token.c_str();
		root["channel"] = channel;
		root["stream_type"] = stream_type;
		root["trans_mode"] = trans_mode;
		root["vsip"] = vsip.c_str();
		root["vsport"] = vsport;
		root["cmd_type"] = cmd_type;
		return root.toStyledString();
	}

	bool jsonparse(std::string strJson)
	{
		Json::Reader reader;  
		// Json::Value是一种很重要的类型，可以代表任意类型。如int, string, object, array...  
		Json::Value root;         

		if (reader.parse(strJson.c_str(), root))  
		{
			cmd_type = root["cmd_type"].asInt();
			dev_id = root["dev_id"].asCString();
			token = root["token"].asCString();
			channel = root["channel"].asInt();
			stream_type = root["stream_type"].asInt();
			trans_mode = root["trans_mode"].asInt();
			vsport = root["vsport"].asInt();
			vsip = root["vsip"].asCString();
		}
		return true;
	}
};
struct dag_play_rsp:http_json_rsp
{
	std::string dev_id;
	int  result;
	int	 channel;
	std::string stream_header;
	std::string sess_id;
public:
	dag_play_rsp()
	{
		result = 0; 
	}
	std::string toJson()
	{
		Json::Value root;
		root["dev_id"] = dev_id.c_str();
		root["stream_header"] = stream_header.c_str();
		root["channel"] = channel;
		root["result"] = result;
		root["sess_id"] = sess_id.c_str();
		return root.toStyledString();
	}

	bool jsonparse(std::string strJson)
	{
		Json::Reader reader;  
		// Json::Value是一种很重要的类型，可以代表任意类型。如int, string, object, array...  
		Json::Value root;         

		if (reader.parse(strJson.c_str(), root))  
		{
			dev_id  = root["dev_id"].asCString();
			channel = root["channel"].asInt();
			result  = root["result"].asInt();
			sess_id = root["sess_id"].asCString();
			stream_header = root["stream_header"].asCString();
		}
		return true;
	}

};

struct cache_vts_info
{
	std::vector<std::string> vts_ip_list;
	std::vector<int> vts_operate_type_list;
	std::vector<int> vts_cmd_port_list;
	std::vector<int> vts_stream_port_list;
	std::string index;
	int update_time;
public:
	cache_vts_info()
	{
		update_time = 0;
	}
	cache_vts_info(const cache_vts_info& cache_vts_info_)
	{
		vts_ip_list = cache_vts_info_.vts_ip_list;
		vts_operate_type_list = cache_vts_info_.vts_operate_type_list;
		vts_cmd_port_list = cache_vts_info_.vts_cmd_port_list;
		vts_stream_port_list = cache_vts_info_.vts_stream_port_list;
		index = cache_vts_info_.index;
		update_time = cache_vts_info_.update_time;

	}
	std::string toJson()
	{
		Json::Value root;
		root["index"] = index;
		Json::Value root_ip_list;
		for (int i = 0;i<vts_ip_list.size();i++)
		{
			Json::Value rootIp;
			rootIp["ip"] = vts_ip_list[i].c_str();
			root_ip_list.append(rootIp);
		}
		root["ip_list"] = root_ip_list;

		Json::Value root_operate_type_list;
		for (int i = 0;i<vts_operate_type_list.size();i++)
		{
			Json::Value root_operate_type;
			root_operate_type["operate_type"] = vts_operate_type_list[i];
			root_operate_type_list.append(root_operate_type);
		}
		root["operate_type_list"] = root_operate_type_list;

		Json::Value root_vts_cmd_port_list;
		for (int i = 0;i<vts_cmd_port_list.size();i++)
		{
			Json::Value root_cmd_port;
			root_cmd_port["cmd_port"] = vts_cmd_port_list[i];
			root_vts_cmd_port_list.append(root_cmd_port);
		}
		root["cmd_port_list"] = root_vts_cmd_port_list;


		Json::Value root_vts_stream_port_list;
		for (int i = 0;i<vts_stream_port_list.size();i++)
		{
			Json::Value root_stream_port;
			root_stream_port["stream_port"] = vts_stream_port_list[i];
			root_vts_stream_port_list.append(root_stream_port);
		}
		root["stream_port_list"] = root_vts_stream_port_list;	
		return root.toStyledString();
	}

	bool jsonparse(std::string strJson)
	{
		Json::Reader reader;  
		// Json::Value是一种很重要的类型，可以代表任意类型。如int, string, object, array...  
		Json::Value root;         
		if (reader.parse(strJson.c_str(), root))  
		{
			index = root["index"].asCString();
			Json::Value arrayObj = root["ip_list"];
			for (int i=0; i<arrayObj.size(); i++)
			{
				vts_ip_list.push_back(arrayObj[i]["ip"].asCString());			 
			}

			arrayObj = root["operate_type_list"];
			for (int i=0; i<arrayObj.size(); i++)
			{
				vts_operate_type_list.push_back(arrayObj[i]["operate_type"].asInt());			 
			}

			arrayObj = root["cmd_port_list"];
			for (int i=0; i<arrayObj.size(); i++)
			{
				vts_cmd_port_list.push_back(arrayObj[i]["cmd_port"].asInt());			 
			}

			arrayObj = root["stream_port_list"];
			for (int i=0; i<arrayObj.size(); i++)
			{
				vts_stream_port_list.push_back(arrayObj[i]["stream_port"].asInt());			 
			}
		}
		return true;
	}

};
struct cache_dev_vts_config
{
	std::string devid;
	std::string dagip;
	int dagport;
	std::vector<std::string> iplist;
	std::vector<int> portlist;
	cache_dev_vts_config()
	{

	}
	cache_dev_vts_config(const cache_dev_vts_config& cache_dev_vts_config_)
	{
		devid = cache_dev_vts_config_.devid;
		dagip = cache_dev_vts_config_.dagip;
		dagport = cache_dev_vts_config_.dagport;
		iplist = cache_dev_vts_config_.iplist;
		portlist = cache_dev_vts_config_.portlist;

	}
	std::string toJson()
	{
		Json::Value dev;
		dev["devid"] = devid.c_str();
		dev["dagip"] = dagip.c_str();
		dev["dagport"] = dagport;
		Json::Value jsonIplist;
		for (int j = 0;j<iplist.size();j++)
		{
			Json::Value ip;
			ip["ip"] = iplist[j].c_str();
			ip["port"] = portlist[j];
			jsonIplist.append(ip);
		}
		dev["iplist"] = jsonIplist;
		return dev.toStyledString();
	}

	bool jsonparse(std::string strJson)
	{
		Json::Reader reader;  
		// Json::Value是一种很重要的类型，可以代表任意类型。如int, string, object, array...  
		Json::Value root;         
		if (reader.parse(strJson.c_str(), root))  
		{
			dagip = root["dagip"].asCString();
			devid = root["devid"].asCString();
			dagport = root["dagport"].asInt();
			Json::Value arrayObjIplist = root["iplist"];
			for (int j = 0;j<arrayObjIplist.size();j++)
			{
				iplist.push_back(arrayObjIplist[j]["ip"].asCString());
				portlist.push_back(arrayObjIplist[j]["port"].asInt());
			}

		}
		return true;
	}


};
struct cache_dev_vts_config_list
{
	cache_dev_vts_config_list()
	{
	}
	cache_dev_vts_config_list(const cache_dev_vts_config_list& cache_dev_vts_config_list_)
	{
		list = cache_dev_vts_config_list_.list;
	}
	std::vector<cache_dev_vts_config> list;
	std::string toJson()
	{
		Json::Value root;
		Json::Value devlist;

		for (int i = 0;i<list.size();i++)
		{
			Json::Value dev;
			dev["devid"] = list[i].devid.c_str();
			dev["dagip"] = list[i].dagip.c_str();
			dev["dagport"] = list[i].dagport;
			Json::Value iplist;
			for (int j = 0;j<list[i].iplist.size();j++)
			{
				Json::Value ip;
				ip["ip"] = list[i].iplist[j].c_str();
				ip["port"] = list[i].portlist[j];
				iplist.append(ip);

			}
			dev["iplist"] = iplist;
			devlist.append(dev);
		}
		root["iplist"] = devlist;
		return root.toStyledString();
	}

	bool jsonparse(std::string strJson)
	{
		Json::Reader reader;  
		// Json::Value是一种很重要的类型，可以代表任意类型。如int, string, object, array...  
		Json::Value root;         
		if (reader.parse(strJson.c_str(), root))  
		{
			Json::Value arrayObj = root["devlist"];
			for (int i =0;i<arrayObj.size();i++)
			{
				cache_dev_vts_config cache_dev_vts_config_;
				cache_dev_vts_config_.dagip = arrayObj[i]["dagip"].asCString();
				cache_dev_vts_config_.devid = arrayObj[i]["devid"].asCString();
				cache_dev_vts_config_.dagport = arrayObj[i]["dagport"].asInt();
				Json::Value arrayObjIplist = arrayObj[i]["iplist"];
				for (int j = 0;j<arrayObjIplist.size();j++)
				{
					cache_dev_vts_config_.iplist.push_back(arrayObjIplist[j]["ip"].asCString());
					cache_dev_vts_config_.portlist.push_back(arrayObjIplist[j]["port"].asInt());
				}
				list.push_back(cache_dev_vts_config_);
			}
		}
		return true;
	}

};
#endif