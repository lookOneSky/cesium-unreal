# 提交 cesium-unreal 到 GitHub v2.15.0 分支

本文档记录把本地 `D:\CodeAll\DasKangLing\Plugins\cesium-unreal` 同步到
`https://github.com/lookOneSky/cesium-unreal` 的操作步骤。

## 当前仓库结构

- 本地 Git 根目录是 `D:\CodeAll\DasKangLing\Plugins`。
- `cesium-unreal` 是这个 Git 仓库里的子目录。
- GitHub 目标仓库的根目录应当直接是 `cesium-unreal` 插件内容，而不是再套一层 `cesium-unreal/`。
- 当前内部 GitLab 远端继续保留为 `origin`。
- GitHub 远端使用单独名称 `github`。

## v2.15.0 tag 和 branch 的区别

GitHub 上已有 `refs/tags/v2.15.0`，这是发布标签，不是分支。

本次需要创建并推送 `refs/heads/v2.15.0` 分支。因为 tag 和 branch 同名，推送时必须使用完整 refspec：

```powershell
git push -u origin HEAD:refs/heads/v2.15.0
```

不要只写下面这种命令：

```powershell
git push origin v2.15.0
```

同名 tag 和 branch 同时存在时，简写容易产生歧义。

## 一次性同步步骤

先进入本地 Git 根目录：

```powershell
Set-Location D:\CodeAll\DasKangLing\Plugins
```

确认本地状态，只处理 `cesium-unreal/`，不要把旁边其它未跟踪目录一起提交：

```powershell
git status --short --branch
```

配置 GitHub 远端。第一次执行：

```powershell
git remote add github https://github.com/lookOneSky/cesium-unreal.git
```

如果已经存在 `github` 远端，改用：

```powershell
git remote set-url github https://github.com/lookOneSky/cesium-unreal.git
```

登录 GitHub 并配置 Git 凭据：

```powershell
gh auth login
gh auth setup-git
gh auth status
```

从 GitHub 的 `v2.15.0` tag 创建临时同步目录，再用本地 tracked 的 `cesium-unreal/` 内容覆盖：

```powershell
$sourceRepo = "D:\CodeAll\DasKangLing\Plugins"
$sourcePrefix = "cesium-unreal"
$syncRoot = Join-Path $env:TEMP ("cesium-unreal-sync-" + (Get-Date -Format yyyyMMddHHmmss))
$archive = Join-Path $env:TEMP "cesium-unreal-current.tar"

git clone https://github.com/lookOneSky/cesium-unreal.git $syncRoot
Set-Location $syncRoot
git checkout -b v2.15.0 refs/tags/v2.15.0

git -C $sourceRepo archive --format=tar HEAD:$sourcePrefix -o $archive
Get-ChildItem -Force $syncRoot | Where-Object { $_.Name -ne ".git" } | Remove-Item -Recurse -Force
tar -xf $archive -C $syncRoot

git status --short
git add -A
git commit -m "Sync local cesium-unreal changes"
git push -u origin HEAD:refs/heads/v2.15.0
```

推送后验证远端分支和 tag：

```powershell
git ls-remote https://github.com/lookOneSky/cesium-unreal.git refs/heads/v2.15.0 refs/tags/v2.15.0
git log --oneline --decorate --max-count=5
git status --short
```

## 后续继续同步的方法

平时仍然在下面目录开发：

```powershell
D:\CodeAll\DasKangLing\Plugins\cesium-unreal
```

本地修改先在 `D:\CodeAll\DasKangLing\Plugins` 这个 Git 仓库里提交。例如：

```powershell
Set-Location D:\CodeAll\DasKangLing\Plugins
git status --short
git add cesium-unreal/<需要提交的文件>
git commit -m "<提交说明>"
```

需要再次发布到 GitHub `v2.15.0` 分支时，重新运行上面的临时同步目录流程。不要直接在 `Plugins` 仓库根目录执行 `git push github master:v2.15.0`，因为那会把 `cesium-unreal/` 作为子目录推上去，不符合 GitHub 目标仓库结构。

如果只是想单独查看 GitHub 上的 `v2.15.0` 分支，可以另选目录克隆：

```powershell
git clone https://github.com/lookOneSky/cesium-unreal.git cesium-unreal-github
Set-Location cesium-unreal-github
git switch v2.15.0
```

## 常见问题

如果推送时提示没有权限，先确认 GitHub CLI 登录状态：

```powershell
gh auth status
```

如果仍然没有权限，说明当前账号可能没有 `lookOneSky/cesium-unreal` 的写权限，需要改为 fork 后提交 PR，或让仓库管理员添加写权限。

也可以查看 Git Credential Manager 当前保存的 GitHub 账号：

```powershell
git credential-manager github list
```

如果看到类似下面的错误，表示当前凭据账号没有目标仓库写权限：

```text
remote: Permission to lookOneSky/cesium-unreal.git denied to <账号名>.
fatal: unable to access 'https://github.com/lookOneSky/cesium-unreal.git/': The requested URL returned error: 403
```

让仓库管理员给该账号添加写权限后，可以直接在临时同步目录重新推送：

```powershell
Set-Location <临时同步目录>
git push -u origin HEAD:refs/heads/v2.15.0
```

如果需要切换到另一个有权限的 GitHub 账号：

```powershell
gh auth login
gh auth setup-git
git credential-manager github login
git credential-manager github list
```

如果本地同时存在 tag 和 branch 名称 `v2.15.0`，推送和拉取时优先使用完整引用：

```powershell
git fetch origin refs/heads/v2.15.0:refs/remotes/origin/v2.15.0
git push origin HEAD:refs/heads/v2.15.0
```
